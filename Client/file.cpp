#include "file.h"
#include "ui_file.h"
#include "client.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
File::File(QWidget *parent) :QWidget(parent),ui(new Ui::File)
{
    ui->setupUi(this);
    m_strCurPath = QString("%1/%2")
            .arg(Client::getInstance().getRootPath())
            .arg(Client::getInstance().m_login_name);
    m_strUserPath = m_strCurPath;
    m_bUpload = false;
    m_bDownload = false;
    flushFile();
    m_pShareFile = new ShareFile;
}


File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::flushFile()
{
    qDebug() <<"flushFile start";
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::updateFileList(QList<FileInfo *> pFileList)
{
    //释放并更新m_fileList
    foreach(FileInfo* pFileInfo, m_fileList){
        delete pFileInfo;
    }
    m_fileList.clear();
    m_fileList = pFileList;
    //清空并更新列表框
    ui->file_LW->clear();
    foreach(FileInfo* pFileInfo, pFileList){
       QListWidgetItem* pItem = new QListWidgetItem;
       if(pFileInfo->iFileType == 0){
           pItem->setIcon(QIcon(QPixmap(":/image/image/file-folder.png")));
       }else if(pFileInfo->iFileType == 1){
           pItem->setIcon(QIcon(QPixmap(":/image/image/file.png")));
       }
       pItem->setText(pFileInfo->caName);
       qDebug() << "pFileInfo->caName " << pFileInfo->caName;
       ui->file_LW->addItem(pItem);
    }

}

void File::uploadFile()
{
    qDebug() << "File::uploadFile start";
    //打开要上传的文件
    QFile file(m_strUploadPath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "上传文件", "打开文件失败");
        return;
    }
    //构建pdu 每次发送4096字节
    m_bUpload = true;
    PDU* datapdu = mkPDU(4096);
    datapdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUEST;
    qint64 ret = 0;
    //循环读取文件的数据，发送给服务器
    qDebug() << "File::uploadFile 11111111";
    while(true){
        ret = file.read(datapdu->caMsg, 4096);
        if(ret == 0){
            qDebug() << "File::uploadFile 22222222";
            break;
        }
        if(ret < 0){
            QMessageBox::warning(this, "上传文件", "上传文件失败：读取失败");
            break;
        }
        //根据ret设置pdu的长度
        datapdu->uiMsgLen = ret;
        datapdu->uiPDULen = ret + sizeof(PDU);
        qDebug() << "File::uploadFile 33333333";
        Client::getInstance().getTcpSocket().write((char*)datapdu, datapdu->uiPDULen);
    }
    m_bUpload = false;
    file.close();
    free(datapdu);
    datapdu = NULL;
}

void File::downloadFile(qint64 iFileSize)
{
    qDebug() << "File::downloadFile start";
    m_iDownloadReceived = 0;
    m_iDownloadTotal = iFileSize;
    m_bDownload = true;
    M_DownloadFile.setFileName(m_strSaveFilePath);
    if(!M_DownloadFile.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this, "下载文件", "打开文件失败");
    }
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA_REQUEST;
    Client::getInstance().sendPDU(pdu);
    qDebug() << "File::downloadFile end";
}

void File::downloadFileData(char *buffer, int size)
{
    qDebug() << "File::downloadFileData start";
    M_DownloadFile.write(buffer, size);
    m_iDownloadReceived += size;
    if(m_iDownloadReceived < m_iDownloadTotal){
        return;
    }
    M_DownloadFile.close();
    m_bDownload = false;
    QMessageBox::information(this, "下载文件", "下载文件完成");
    qDebug() << "File::downloadFileData end";
}

ShareFile *File::getShareFile()
{
    return m_pShareFile;
}

void File::on_mkDir_PB_clicked()
{
    QString strNewDir = QInputDialog::getText(this, "新建文件夹", "文件夹名：");
    if(strNewDir.isEmpty() || strNewDir.toStdString().size() > 32){
        QMessageBox::warning(this, "新建文件夹", "文件夹名长度非法");
        return;
    }
    //文件夹名放入caData，当前路径放入caMsg
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_REQUEST;
    memcpy(pdu->caData, strNewDir.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::on_flushFile_PB_clicked()
{
    flushFile();
}

void File::on_delDir_PB_clicked()
{
    //获取选中的文件夹名
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "删除文件夹", "请选择要删除的文件夹");
        return;
    }
    QString strDelFileName = pItem->text();
    //判断选择的是否为文件夹
    foreach(FileInfo* pFileInfo, m_fileList){
        if(pFileInfo->caName == strDelFileName && pFileInfo->iFileType != 0){
            QMessageBox::warning(this, "删除文件夹", "选择的不是文件夹");
            return;
        }
    }
    //确认是否删除
    int ret = QMessageBox::question(this, "删除文件夹", QString("是否确定删除文件夹%1").arg(strDelFileName));
    if(ret != QMessageBox::Yes){
        return;
    }
    //完整路径发送给服务器
    QString strPath = m_strCurPath + "/" + strDelFileName;
    PDU* pdu = mkPDU(strPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
    memcpy(pdu->caMsg, strPath.toStdString().c_str(), strPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::on_delFile_PB_clicked()
{
    //获取选中的文件名
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "删除文件", "请选择要删除的文件");
        return;
    }
    QString strDelFileName = pItem->text();
    //判断选择的是否为文件
    foreach(FileInfo* pFileInfo, m_fileList){
        if(pFileInfo->caName == strDelFileName && pFileInfo->iFileType != 1){
            QMessageBox::warning(this, "删除文件", "选择的不是文件");
            return;
        }
    }
    //确认是否删除
    int ret = QMessageBox::question(this, "删除文件", QString("是否确定删除文件%1").arg(strDelFileName));
    if(ret != QMessageBox::Yes){
        return;
    }
    //完整路径发送给服务器
    QString strPath = m_strCurPath + "/" + strDelFileName;
    PDU* pdu = mkPDU(strPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUEST;
    memcpy(pdu->caMsg, strPath.toStdString().c_str(), strPath.toStdString().size());
    qDebug() << "pdu->caMsg:" << pdu->caMsg;
    Client::getInstance().sendPDU(pdu);
}

void File::on_rename_PB_clicked()
{
    //获取选中的文件夹名
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "重命名文件", "请选择要重命名的文件");
        return;
    }
    QString strOldFileName = pItem->text();
    QString strNewFileName = QInputDialog::getText(this, "重命名文件", "重命名文件名：");
    if(strNewFileName.isEmpty() || strNewFileName.toStdString().size() > 32){
        QMessageBox::warning(this, "重命名文件", "文件名长度非法");
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
    memcpy(pdu->caData, strOldFileName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strNewFileName.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);
}

void File::on_file_LW_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();
    //判断选择的是否为文件夹
    foreach(FileInfo* pFileInfo, m_fileList){
        if(pFileInfo->caName == strDirName && pFileInfo->iFileType != 0){
            QMessageBox::warning(this, "提示", "选择的不是文件夹");
            return;
        }
    }
    m_strCurPath = m_strCurPath + "/" + strDirName;
    flushFile();
}

void File::on_return_PB_clicked()
{
    if(m_strCurPath == m_strUserPath){
        QMessageBox::warning(this, "返回", "返回失败，已在顶层目录");
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index, m_strCurPath.size() - index);
    flushFile();
}

void File::on_mvFile_PB_clicked()
{
    if(ui->mvFile_PB->text() == "移动文件"){
        QListWidgetItem* pItem = ui->file_LW->currentItem();
        //判断
        if(pItem == NULL){
            QMessageBox::warning(this, "移动文件", "请选择要移动的文件");
            return;
        }
        //提示用户要移动的目录
        QMessageBox::information(this, "移动文件", "请选择要移动到的目录");
        //按钮文字改为确认或取消
        ui->mvFile_PB->setText("确认/取消");
        //文件名和完整路径保存
        m_strMvFileName = pItem->text();
        m_strMvPath = m_strCurPath + "/" + m_strMvFileName;
        return;
    }
    //点击确认或取消后

    //获取当前文件
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    QString strTarPath;
    QString boxMsg;
    if(pItem == NULL){
        strTarPath = m_strCurPath + "/" + m_strMvFileName;
        boxMsg = QString("是否移动到当前目录下");
    }else {
        //判断选择的是否为文件夹
        foreach(FileInfo* pFileInfo, m_fileList){
            if(pFileInfo->caName == pItem->text() && pFileInfo->iFileType != 0){
                QMessageBox::warning(this, "提示", "选择的不是文件夹");
                return;
            }
        }
        strTarPath = m_strCurPath + "/" + pItem->text() + "/" + m_strMvFileName;
        boxMsg = QString("是否移动到已选中目录下");
    }
    int ret = QMessageBox::information(this, "移动文件", boxMsg, "确认", "取消");
    ui->mvFile_PB->setText("移动文件");
    if(ret != 0){//说明取消了
        return;
    }
    int srcLen = m_strMvPath.toStdString().size();
    int tarLen = strTarPath.toStdString().size();
    PDU* pdu = mkPDU(srcLen + tarLen + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
    memcpy(pdu->caData, &srcLen, sizeof(int));
    memcpy(pdu->caData + sizeof(int), &tarLen, sizeof(int));

    memcpy(pdu->caMsg, m_strMvPath.toStdString().c_str(), srcLen);
    memcpy(pdu->caMsg + srcLen, strTarPath.toStdString().c_str(), tarLen);
    Client::getInstance().sendPDU(pdu);

}

void File::on_upload_PB_clicked()
{
    if(m_bUpload){
        QMessageBox::warning(this, "上传文件", "已有文件正在上传，请稍等");
        return;
    }
//    m_strUploadPath.clear();
    m_strUploadPath = QFileDialog::getOpenFileName();
    qDebug() << "m_strUploadPath:" << m_strUploadPath;
    int index = m_strUploadPath.lastIndexOf('/');
    QString strFileName = m_strUploadPath.right(m_strUploadPath.size() - index - 1);
//    qDebug() << "m_strUploadPath.toStdString().size():" << m_strUploadPath.toStdString().size();
//    qDebug() << "m_strUploadPath.size():" << m_strUploadPath.size();
//    qDebug() << "index:" << index;
    qDebug() << "strFileName:" << strFileName;
    QFile file(m_strUploadPath);
    qint64 fileSize = file.size();

    PDU* pdu = mkPDU(m_strCurPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
    memcpy(pdu->caData, strFileName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, &fileSize, sizeof(qint64));
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(),m_strCurPath.toStdString().size());
    Client::getInstance().sendPDU(pdu);

}

void File::on_download_PB_clicked()
{
    if(m_bUpload){
        QMessageBox::warning(this, "下载文件", "已有文件正在下载");
        return;
    }
    QListWidgetItem *pItem = ui->file_LW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "下载文件", "请选择要下载的文件");
        return;
    }
    m_strSaveFilePath.clear();
    m_strSaveFilePath = QFileDialog::getSaveFileName();
    if(m_strSaveFilePath.isEmpty()){
        QMessageBox::warning(this, "下载文件", "请指定下载路径");
        return;
    }
    m_strDownloadFileName = pItem->text();
    qDebug() << "m_strDownloadFileName:" << m_strDownloadFileName;
    QString strPath = QString("%1/%2").arg(m_strCurPath).arg(m_strDownloadFileName);
    qDebug() << "strPath:" << strPath;
    PDU* pdu = mkPDU(strPath.toStdString().size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    memcpy(pdu->caMsg, strPath.toStdString().c_str(), strPath.toStdString().size());
    qDebug() << "downloadFile pdu->caMsg:" << pdu->caMsg;
    Client::getInstance().sendPDU(pdu);
}

void File::on_shareFile_PB_clicked()
{
    //获取选中的文件夹名
    QListWidgetItem* pItem = ui->file_LW->currentItem();
    if(pItem == NULL){
        QMessageBox::warning(this, "分享文件", "请选择要分享的文件");
        return;
    }
    m_strShareFileName = pItem->text();
    m_pShareFile->updateFriend_LW();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }
}
