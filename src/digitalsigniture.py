import base64
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_v1_5 as PKCS1_Cipher
from Crypto.Signature import PKCS1_v1_5 as PKCS1_Signer
from Crypto import Random
from Crypto.Hash import SHA


def get_key_pair():
    random = Random.new().read
    key = RSA.generate(1024, random)
    pubkey = key.publickey().exportKey('PEM')
    prikey = key.exportKey('PEM')
    return pubkey, prikey


def encrypt(pubkey, message):
    message = bytes(message.encode("utf-8"))
    rsakey = RSA.importKey(pubkey)
    cipher = PKCS1_Cipher.new(rsakey)
    encrypted = cipher.encrypt(message)
    encrypted = base64.b64encode(encrypted)
    return encrypted


def decrypt(prikey, message):
    random = Random.new().read
    rsakey = RSA.importKey(prikey)
    cipher = PKCS1_Cipher.new(rsakey)
    decrypted = cipher.decrypt(base64.b64decode(message), random)
    return decrypted.decode('utf-8')


pubkey, prikey = get_key_pair()
print(prikey)

encrypted = encrypt(pubkey, "Oh Mather fucker!")

print(encrypted)

print(pubkey)

decrypted = decrypt(prikey, encrypted)

print(decrypted)


def sign(prikey, message):
    message = message.encode("utf-8")
    rsakey = RSA.importKey(prikey)
    signer = PKCS1_Signer.new(rsakey)
    digest = SHA.new()
    digest.update(message)
    signature = signer.sign(digest)
    signature = base64.b64encode(signature)
    return signature


def verify(pubkey, message, signature):
    message = message.encode("utf-8")
    rsakey = RSA.importKey(pubkey)
    verifier = PKCS1_Signer.new(rsakey)
    digest = SHA.new()
    digest.update(message)
    result = verifier.verify(digest, base64.b64decode(signature))
    return result


signature = sign(prikey, "Oh Mather fucker!")

print(signature)
vfy = verify(pubkey, "Oh Mather fucker!", signature)
print(vfy)