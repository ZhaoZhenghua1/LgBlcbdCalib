#pragma once
#include <shared_mutex>
#include "RegisterEx.h"
#include "Decoder.h"
#include "..\stdafx.h"

struct DataInit
{
	CString CLOUDIP, CLOUDPORT, RtmpPort, CameraUserName, CameraUserPwd, SHOWRATE, SPEED;
	CString WS_SysMgr_G_GetSchoolBaseInfo, WS_G_GetSubSystemServerInfo, WS_IDMC_Room_Detail_Electric_Get;
	void InitFromRegedit();
	void InitFromWebService();
	CString GetCameraIP() { return m_cameraIP; }
private:
	CString GetCameraIP(const CString&);
	CString m_cameraIP;
};
 
class CameraThings
{
public:
	void Init();
	void StartCamera(bool b);
	Mat GetImage();
	void SetImage(const Mat& m);
	Decoder& GetDecoder() { return m_decoder; }
	bool IsRunning() { return m_running; }
	CString GetCameraIP() { return m_initData.GetCameraIP(); }
	void Save(EBBStatus eBBType, CRect (*)[4], CSize );
	CRect(*GetDataFromRegedit(EBBStatus eBBType, CSize displaySize))[4];
private:
	DataInit m_initData;
	Decoder m_decoder;
	Mat m_imageOut;
	std::shared_mutex m_lock;
	bool m_running = false;
};

