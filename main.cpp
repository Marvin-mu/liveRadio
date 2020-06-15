#include "mainwindow.h"
#include <windows.h>
#include <dbghelp.h>
#include <QDateTime>
#include <QTextCodec>

LONG CreateCrashHandler(EXCEPTION_POINTERS *pException){
    //���� Dump �ļ�
    QDateTime CurDTime = QDateTime::currentDateTime();
    QString current_date = CurDTime.toString("yyyy_MM_dd_hh_mm_ss");
    //dmp�ļ�������
    QString dumpText = "Dump_"+current_date+".dmp";
    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    //QString errAddr(QString::number(uint)record->ExceptionAddress, 16));
    qDebug() << record->ExceptionAddress;
    QString errFlag(QString::number(record->ExceptionFlags, 16));
    QString errPara(QString::number(record->NumberParameters, 16));
    HANDLE DumpHandle = CreateFile((LPCWSTR)dumpText.utf16(),
             GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(DumpHandle != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //��dump��Ϣд��dmp�ļ�
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),DumpHandle, MiniDumpNormal, &dumpInfo, NULL, NULL);
        CloseHandle(DumpHandle);
    }
    //������Ϣ��ʾ
    QMessageBox::warning(NULL,"Dump",QString("ErrorCode%1 ErrorFlag:%2 ErrorPara:%3").arg(errCode).arg(errFlag).arg(errPara),
        QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //��ȡϵͳ����
    //codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //ע���쳣������
    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CreateCrashHandler);
    MainWindow w;
    w.show();
    return a.exec();
}
