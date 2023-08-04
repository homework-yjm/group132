import binascii
from gmssl import sm2
from Cryptodome.Cipher import AES

class PGP:
    def __init__(self,msg):#
        self.iv=b'0000000000000000'
        self.key_AES=b'0zyy201900460044' #设置为会话密钥
        self.crypt_AES=AES.new(self.key_AES, AES.MODE_CBC, self.iv)

        #提前计算好公钥和私钥内容（sm2）
        self.pri_key = '66eb9263d349f472971aba2254ea19381c17fd462193d9fb12361335b2ae634f'
        self.pub_key = '677f138898e9fef7e85227822f4a76208b508ebfc03c9682deb7488efa86f8fbbba074f66fcbd3fe08cd2cd4745e3e92c5ba64c626261bcc58279e6b9639f310'

        self.sm2_crypt = sm2.CryptSM2(public_key=self.pub_key, private_key=self.pri_key)
        self.message=msg

    #整个的PGP加密过程
    def encode(self):
        enced_key_sys=self.encrypt_key(self.key_AES)
        enced_m=self.encrypt_massage(self.message)
        final=self.link(enced_key_sys,enced_m)
        mss_date=self.decode_from_byte(final)
        return mss_date

    #对于会话密钥进行sm2加密
    def encrypt_key(self,key_AES):
        enc_data = self.sm2_crypt.encrypt(key_AES)
        return enc_data

    #对于消息进行AES加密
    def encrypt_massage(self,m):
        message_byte=bytes(m,encoding='utf-8')
        encrypt_value=self.crypt_AES.encrypt(message_byte) 
        return encrypt_value

    #将两部分内容进行了连接
    def link(self,enc_sys_key,enc_m):
        final_mss=enc_sys_key+enc_m
        return final_mss
    
    #进行base64转化后decode
    def decode_from_byte(self,final_mss):
        final_data=binascii.b2a_base64(final_mss).decode('utf-8')
        return final_data

message="yanjiamiaosdu2021"
final_mss=PGP(message).encode()
print(final_mss)
