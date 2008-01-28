#ifndef DOWNLOADER_P_H
#define DOWNLOADER_P_H

#include <QtCore/QThread>
#include <curl/curl.h>

class MyThread : public QThread
{
  Q_OBJECT
public:
  MyThread(CURL *handle, QObject *parent = 0);
  void cancel();
  CURLcode retCode() const;
Q_SIGNALS:
  void progress(double dltotal, double dlnow);
protected:
  virtual void run();

  int progressCallback(double ultotal, double ulnow);
  static int curlProgressCallback(void *clientp, double dltotal, double dlnow,
                                  double ultotal, double ulnow);
protected:
  CURL    *curlHandle;
  bool     m_bCancel;
  CURLcode m_ret;
};

#endif  // DOWNLOADER_P_H
