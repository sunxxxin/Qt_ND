#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef  unsigned int uint;

enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN = 0,
    //注册
    ENUM_MSG_TYPE_REGIST_REQUEST,
    ENUM_MSG_TYPE_REGIST_RESPEND,
    //登录
    ENUM_MSG_TYPE_LOGIN_REQUEST,
    ENUM_MSG_TYPE_LOGIN_RESPEND,
    //查找用户
    ENUM_MSG_TYPE_FINDUSER_REQUEST,
    ENUM_MSG_TYPE_FINDUSER_RESPEND,
    //在线用户
    ENUM_MSG_TYPE_ONLINEUSER_REQUEST,
    ENUM_MSG_TYPE_ONLINEUSER_RESPEND,
    //添加好友
    ENUM_MSG_TYPE_ADDFRIEND_REQUEST,
    ENUM_MSG_TYPE_ADDFRIEND_RESPEND,
    //同意添加好友
    ENUM_MSG_TYPE_ADDFRIEND_AGREE_REQUEST,
    ENUM_MSG_TYPE_ADDFRIEND_AGREE_RESPEND,
    //刷新在线好友
    ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_REQUEST,
    ENUM_MSG_TYPE_FLUSH_ONLINE_FRIEND_RESPEND,
    //删除在线好友
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPEND,
    //聊天
    ENUM_MSG_TYPE_CHAT_REQUEST,
    ENUM_MSG_TYPE_CHAT_RESPEND,
    //新建文件夹
    ENUM_MSG_TYPE_MKDIR_REQUEST,
    ENUM_MSG_TYPE_MKDIR_RESPEND,
    //刷新文件
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,
    ENUM_MSG_TYPE_FLUSH_FILE_RESPEND,
    //删除文件夹
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,
    ENUM_MSG_TYPE_DEL_DIR_RESPEND,
    //删除文件
    ENUM_MSG_TYPE_DEL_FILE_REQUEST,
    ENUM_MSG_TYPE_DEL_FILE_RESPEND,
    //重命名文件
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,
    ENUM_MSG_TYPE_RENAME_FILE_RESPEND,
    //移动文件
    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,
    ENUM_MSG_TYPE_MOVE_FILE_RESPEND,
    //上传文件
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPEND,
    //上传文件数据
    ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST,
    ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPEND,
    //下载文件
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPEND,
    //下载文件数据
    ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST,
    ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_RESPEND,
    //分享文件
    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,
    ENUM_MSG_TYPE_SHARE_FILE_RESPEND,
    //同意分享文件
    ENUM_MSG_TYPE_SHARE_FILE_AGREE_REQUEST,
    ENUM_MSG_TYPE_SHARE_FILE_AGREE_RESPEND,
    ENUM_MSG_TYPE_MAX = 0x00fffff,
};


struct PDU{//协议数据单元结构体
    uint uiPDULen;//总的协议数据长度
    uint uiMsgLen;//实际消息的长度
    uint uiMsgType;//消息类型
    char caData[64];//参数
    char caMsg[];//实际消息


};


struct FileInfo{
    char caName[32];
    int iFileType;

};

//构建协议数据单元
PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
