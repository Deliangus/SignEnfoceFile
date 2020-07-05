import os
import sys
import hashlib

import time
root_folder_path = os.path.join(os.getcwd(), "Enforcement").replace('\\', '/')
database = os.path.join(os.getcwd(), "database")
filetree = os.path.join(os.getcwd(), "filetree")
log_file = os.path.join(os.getcwd(), "file_change_log")


class Folder:
    def __init__(self, root):
        self.root = root.replace('\\', '/')
        self.name = os.path.basename(root)
        self.folders = {}
        self.files = {}
        self.diff = {}
        #self.folders, self.files = self.build_file_tree(root)

    def build_file_tree(self):
        self.folders = {}
        self.files = {}
        if (os.path.exists(self.root)):
            for filename in os.listdir(self.root):
                filepath = os.path.join(self.root, filename)
                if (os.path.isfile(filepath)):
                    self.files[filename] = get_file_md5(filepath)
                elif (os.path.isdir(filepath)):
                    self.folders[filename] = Folder(filepath)
                    self.folders[filename].build_file_tree()
                else:
                    print(filepath, "build_file_tree error")

    def add_file(self, filepath, md5val):

        filepath = filepath.replace('\\', '/')

        if (self.root == filepath[:len(self.root)]):

            path = filepath.split('/')[len(self.root.split('/')):]

            if (len(path) == 1):
                self.files[path[0]] = md5val
                #print(self.root, path)
                #print(self.files)
                #print(self.folders)
            else:
                if (path[0] in self.folders):
                    pass
                else:
                    self.folders[path[0]] = Folder(
                        os.path.join(self.root, path[0]))
                self.folders[path[0]].add_file(filepath, md5val)

        pass

    def verify_file_tree(self):
        if (os.path.exists(self.root)):
            filenames = os.listdir(self.root)
        else:
            filenames = []

        #print(self.root)
        #print(self.diff)

        for filename in filenames:
            #print(self.root, filename)
            filepath = os.path.join(self.root, filename).replace('\\', '/')
            if (os.path.isfile(filepath)):
                realmd5 = get_file_md5(filepath)
                if (filename in self.files):
                    if (realmd5 != self.files[filename]):
                        self.diff[filepath] = {
                            "Operation": "CHANGE",
                            "Operator": "VERIFY",
                            "Time": get_time(),
                            "Current_MD5": realmd5,
                            "Old_MD5": self.files[filename],
                            "Current_name": filename,
                        }
                else:
                    for x, md5val in self.files.items():
                        if (x not in filenames and md5val == realmd5):
                            self.diff[filepath] = {
                                "Operation": "RENAME",
                                "Operator": "VERIFY",
                                "Time": get_time(),
                                "Current_MD5": realmd5,
                                "Old_MD5": md5val,
                                "Current_name": filename,
                                "Old_Name": x
                            }
                            break

                    if (filepath not in self.diff):
                        self.diff[filepath] = {
                            "Operation": "CREATE",
                            "Operator": "VERIFY",
                            "Time": get_time(),
                            "Current_MD5": realmd5,
                            "Current_name": filename,
                        }
            else:
                if (filename in self.folders):
                    self.folders[filename].verify_file_tree()
                else:
                    self.folders[filename] = Folder(filepath)
                    self.folders[filename].verify_file_tree()

        for filename, md5val in self.files.items():
            filepath = os.path.join(self.root, filename).replace('\\', '/')
            if (filename not in filenames):
                unprocessed = True
                for key, value in self.diff.items():
                    print(value)
                    if (md5val == value["Current_MD5"]
                            and filename == value["Old_Name"]):
                        unprocessed = False
                        break
                if (unprocessed and filepath not in self.diff):
                    self.diff[filepath] = {
                        "Operation": "DELETE",
                        "Operator": "VERIFY",
                        "Time": get_time(),
                        "Current_MD5": realmd5,
                        "Current_name": filename,
                    }

        for filename, folder in self.folders.items():
            if (filename not in filenames):
                #print(self.folders.keys())
                #print(filenames)
                #input()
                self.folders[filename].verify_file_tree()

    def save_verfiy_log(self):
        fields = [
            "Operation", "Operator", "Time", "Current_MD5", "Old_MD5",
            "Current_Name", "Old_Name"
        ]

        with open(log_file, 'a') as fout:
            for key, ct in self.diff.items():
                toWrite = []
                for field in fields:
                    if (field in ct):
                        toWrite.append(ct[field])
                    else:
                        toWrite.append('')
                fout.write(key + "," + ",".join(toWrite) + "\n")

        for foldername, folder in self.folders.items():
            folder.save_verfiy_log()

    def save(self, file):
        for filename, md5val in self.files.items():
            file.write(",".join([
                os.path.join(self.root, filename).replace('\\', '/'), md5val
            ]) + "\n")

        for filename, folder in self.folders.items():
            self.folders[filename].save(file)

    def read_file_tree(self, filepath):
        if (os.path.exists(filepath)):
            with open(filepath, 'r') as fin:
                lines = fin.readlines()
                for line in lines:
                    line = line.strip().split(',')
                    path = line[0][len(root_folder_path) + 1:].split('/')
                    md5val = line[1]
                    self.add_file(line[0], line[1])
                    #print(self.root)
                    #print(len(self.folders), len(self.files))
        else:
            print(filepath + " Not Found.")

    def read_pending_operation(self, filepath):
        items = {}
        with open(filepath, 'r') as fin:
            for line in fin.readlines():
                line = line.strip().split(',').replace('\\', '/')
                items[line[0]] = {
                    "Operation": line[1],
                    "Operator": line[2],
                    "Time": line[3],
                    "Current_MD5": line[4],
                    "Old_MD5": line[5],
                    "Current_Name": line[6],
                    "Old_Name": line[7],
                    "Public_Key_ID": line[8],
                }

        for key, ct in items.items():
            md5val = get_file_md5(key)


def get_time():
    import time
    return time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())


def get_file_md5(filepath):
    if os.path.isfile(filepath):
        mb5gen = hashlib.md5()
        maxbuf = 8192
        f = open(filepath, 'rb')
        while True:
            buf = f.read(maxbuf)
            if not buf:
                break
            mb5gen.update(buf)
        f.close()
        md5val = mb5gen.hexdigest()
        return str(md5val).upper()
    return ""


def get_md5(string):
    mb5gen = hashlib.md5()
    try:
        mb5gen.update(string)
    except Exception as e:
        mb5gen.update(string.encode("utf-8"))

    md5val = mb5gen.hexdigest()
    return str(md5val).upper()


def read_database():
    filedata = {}
    if (os.path.exists(database)):
        with open(database, 'r') as fin:
            pass


if __name__ == '__main__':
    while True:
        root_folder = Folder(root_folder_path)
        root_folder.read_file_tree(database)
        root_folder.read_pending_operation()
        root_folder.verify_file_tree()
        root_folder.save_verfiy_log()
        root_folder.build_file_tree()
        with open(database, 'w') as fout:
            root_folder.save(fout)
        time.sleep(5)
