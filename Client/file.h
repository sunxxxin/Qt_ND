#ifndef FILE_H
#define FILE_H

#include <QWidget>
#include <protocol.h>
#include <QListWidgetItem>
#include <QFile>
#include <sharefile.h>
namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    //用户的当前目录
    QString m_strCurPath;
    //用户的根目录，不变
    QString m_strUserPath;
    void flushFile();
    QList<FileInfo*> m_fileList;
    void updateFileList(QList<FileInfo*> pFileList);
    QString m_strMvFileName;
    QString m_strMvPath;
    //上传文件路径
    QString m_strUploadPath;
    //是否正在上传
    bool m_bUpload;
    void uploadFile();

    QString m_strShareFileName;

    //下载文件路径
    QString m_strSaveFilePath;
    //下载文件名称
    QString m_strDownloadFileName;
    void downloadFile(qint64 iFileSize);
    qint64 m_iDownloadReceived;
    qint64 m_iDownloadTotal;
    bool m_bDownload;
    QFile M_DownloadFile;
    void downloadFileData(char* buffer, int size);
    ShareFile* getShareFile();
private slots:
    void on_mkDir_PB_clicked();

    void on_flushFile_PB_clicked();

    void on_delDir_PB_clicked();

    void on_delFile_PB_clicked();

    void on_rename_PB_clicked();

    void on_file_LW_itemDoubleClicked(QListWidgetItem *item);

    void on_return_PB_clicked();

    void on_mvFile_PB_clicked();

    void on_upload_PB_clicked();

    void on_download_PB_clicked();

    void on_shareFile_PB_clicked();

private:
    Ui::File *ui;
    ShareFile* m_pShareFile;
};

#endif // FILE_H
