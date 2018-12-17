#include "CameraThings.h"
#include <string>
#include "..\rapidJson.h"
#include "..\include\pugixml\pugixml.hpp"

std::wstring s2ws(const std::string& str)
{
	int len;
	int slength = (int)str.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, &r[0], len);
	return r;
}

// Convert a wide Unicode string to an UTF8 string
std::string ws2s(const std::wstring &wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

const CString RIGISTER_KEY = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths");
CString FromBSTR(BSTR bstr)
{
	CString ret = bstr;
	::SysFreeString(bstr);
	return ret;
}
CString HttpGet(const CString& url)
{
	auto dllHandle = LoadLibrary(L"CCSInteroperate.dll");
	typedef BSTR(WINAPI *PHttpGetWebService)(const wchar_t *);
	static PHttpGetWebService fun = (PHttpGetWebService)GetProcAddress(dllHandle, "HttpGetWebService");
	if (!fun)
	{
		throw std::wstring(L"load function HttpGetWebService in CCSInteroperate.dll failed!");
	}
	
	CString ret = FromBSTR(fun(url));
	WDocument doc;
	rapidjson::ParseResult ok = doc.Parse(ret);
	if (doc.HasParseError())
	{
		throw std::wstring(ret);
	}

	if (doc[L"Status"].GetInt() != 0)
	{
		throw std::wstring(doc[L"Msg"].GetString());
	}
	return doc[L"Data"].GetString();
}

void DataInit::InitFromRegedit()
{
	CString strSubKey = RIGISTER_KEY + CString(_T("\\CloudClassroom\\ProcParam"));

	CLOUDIP = getstrValueFromRegedit(strSubKey, L"CLOUDIP");
	CLOUDPORT = getstrValueFromRegedit(strSubKey, L"CLOUDPORT");

	RtmpPort = getstrValueFromRegedit(strSubKey, L"RtmpPort");
	if (RtmpPort == TEXT(""))
		RtmpPort = TEXT("9911");

	CameraUserName = getstrValueFromRegedit(strSubKey, L"CameraUserName");
	if (CameraUserName == TEXT(""))
		CameraUserName = TEXT("admin");

	CameraUserPwd = getstrValueFromRegedit(strSubKey, L"CameraUserPwd");
	if (CameraUserPwd == TEXT(""))
		CameraUserPwd = TEXT("lg123456");


	strSubKey = RIGISTER_KEY + CString(_T("\\CloudClassroom\\ProcParam"));
	CString SHOWRATE = getstrValueFromRegedit(strSubKey, L"SHOWRATE");
	if (SHOWRATE == TEXT(""))
		SHOWRATE = TEXT("0.3");

	strSubKey = RIGISTER_KEY + CString(_T("\\CloudClassroom\\ProcParam"));
	SPEED = getstrValueFromRegedit(strSubKey, L"SPEED");
	if (SPEED == TEXT(""))
		SPEED = TEXT("3");
}

void DataInit::InitFromWebService()
{
	WS_SysMgr_G_GetSchoolBaseInfo = HttpGet(L"http://" + CLOUDIP + L":" + CLOUDPORT + L"/SysMgr/SysSetting/WS/Service_SysSetting.asmx/WS_SysMgr_G_GetSchoolBaseInfo?schoolID=""");
	WS_G_GetSubSystemServerInfo = HttpGet(L"http://" + CLOUDIP + L":" + CLOUDPORT + L"/Base/WS/Service_Basic.asmx/WS_G_GetSubSystemServerInfo?sysID=" + L"E00" + L"&subjectID=" + L"");
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_string(WS_G_GetSubSystemServerInfo);
	if (!res)
	{
		throw std::wstring(L"http://" + CLOUDIP + L":" + CLOUDPORT + L"/Base/WS/Service_Basic.asmx/WS_G_GetSubSystemServerInfo?sysID=" + L"E00" + L"&subjectID=" + L"") + L" WEB API GET XML PARSE ERROR!" + s2ws(res.description());
	}
	auto selected = doc.select_nodes(L"/ArrayOfString/string[3]");
	if (selected.empty())
	{
		throw std::wstring(L"/ArrayOfString/string[3]") + L" does not exist in WS_G_GetSubSystemServerInfo WEB API";
	}
	CString EIpAndPort = selected[0].node().child_value();
	WS_IDMC_Room_Detail_Electric_Get = HttpGet(EIpAndPort + L"WebService/Mgr_Classroom_PC.asmx/WS_IDMC_Room_Detail_Electric_Get");
	//L"rtsp://admin:lg123456@172.16.63.42/live/av0";L"rtsp://admin:lg123456@172.16.63.65:554";//临时代码
	m_cameraIP = L"rtsp://admin:lg123456@172.16.63.165:554";//GetCameraIP(WS_IDMC_Room_Detail_Electric_Get) + L"/live/av0";//
}

CString DataInit::GetCameraIP(const CString& WS_IDMC_Room_Detail_Electric_Get)
{
	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_string(WS_IDMC_Room_Detail_Electric_Get);
	if (!res)
	{
		throw std::wstring(L"WS_IDMC_Room_Detail_Electric_Get") + L" WEB API GET XML PARSE ERROR!" + s2ws(res.description());
	}

	auto selected = doc.select_nodes(L"/ArrayOfModel_Room_Detail_Electric_Get/Model_Room_Detail_Electric_Get/cameraInfo/Model_CameraInfo[camType='3' and camName='Tea1']/camInfo");//or camType='2' and
	
	if (selected.empty())
	{
		throw std::wstring(L"/ArrayOfModel_Room_Detail_Electric_Get/Model_Room_Detail_Electric_Get/cameraInfo/Model_CameraInfo[camType='3' and camName='Tea1']/camInfo") + L"does not exist in WS_IDMC_Room_Detail_Electric_Get WEB API。无法获取到摄像机IP";
	}
	CString camInfo = selected[0].node().child_value();
	//url = "rtsp://" + infos[2] + ":" + infos[3] + "@" + infos[0] + ":" + infos[1];
	CStringArray lstInfo;
	camInfo += "||||";
	int begin = 0;
	for (int i = camInfo.Find(L"|", begin); i >= 0;)
	{
		lstInfo.Add(camInfo.Mid(begin, i - begin));
		begin = i + 1;
		i = camInfo.Find(L"|", begin);
	}
	return "rtsp://" + lstInfo[2] + ":" + lstInfo[3] + "@" + lstInfo[0] + ":" + lstInfo[1];
}

void MatToCImage(Mat& mat, CImage& cimage)
{
	if (0 == mat.total())
	{
		return;
	}

	int nChannels = mat.channels();
	if ((1 != nChannels) && (3 != nChannels))
	{
		return;
	}
	int nWidth = mat.cols;
	int nHeight = mat.rows;


	//重建cimage
	cimage.Destroy();
	cimage.Create(nWidth, nHeight, 8 * nChannels);


	//拷贝数据


	uchar* pucRow;									//指向数据区的行指针
	uchar* pucImage = (uchar*)cimage.GetBits();		//指向数据区的指针
	int nStep = cimage.GetPitch();					//每行的字节数,注意这个返回值有正有负


	if (1 == nChannels)								//对于单通道的图像需要初始化调色板
	{
		RGBQUAD* rgbquadColorTable;
		int nMaxColors = 256;
		rgbquadColorTable = new RGBQUAD[nMaxColors];
		cimage.GetColorTable(0, nMaxColors, rgbquadColorTable);
		for (int nColor = 0; nColor < nMaxColors; nColor++)
		{
			rgbquadColorTable[nColor].rgbBlue = (uchar)nColor;
			rgbquadColorTable[nColor].rgbGreen = (uchar)nColor;
			rgbquadColorTable[nColor].rgbRed = (uchar)nColor;
		}
		cimage.SetColorTable(0, nMaxColors, rgbquadColorTable);
		delete[]rgbquadColorTable;
	}


	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pucRow = (mat.ptr<uchar>(nRow));
		for (int nCol = 0; nCol < nWidth; nCol++)
		{
			if (1 == nChannels)
			{
				*(pucImage + nRow * nStep + nCol) = pucRow[nCol];
			}
			else if (3 == nChannels)
			{
				for (int nCha = 0; nCha < 3; nCha++)
				{
					*(pucImage + nRow * nStep + nCol * 3 + nCha) = pucRow[nCol * 3 + nCha];
				}
			}
		}
	}
}

void CameraThread(void* obj)
{
	CameraThings* pOut = (CameraThings*)obj;
	while (pOut->IsRunning())
	{
		Mat img;
		if (pOut->GetDecoder().getImg(img) > 0)
		{
			if (!img.empty())
			{
				pOut->SetImage(img);
			}
		}
		Sleep(5);
	}
}

void CameraThings::Init()
{
	m_initData.InitFromRegedit();
	m_initData.InitFromWebService();
	CString csip = m_initData.GetCameraIP();
	const wchar_t* ip = csip;
	m_decoder.initDecoder(ws2s(ip));
}

void CameraThings::StartCamera(bool b)
{
	m_running = b;
	if (b)
	{
		_beginthread(&CameraThread, 0, this);
	}
}

Mat CameraThings::GetImage()
{
	std::shared_lock<std::shared_mutex> rangedLocker(m_lock);
	return m_imageOut;
}

void CameraThings::SetImage(const Mat & m)
{
	std::unique_lock<std::shared_mutex> rangedLocker(m_lock);
	m_imageOut = m;
}
//
//void opencamera()
//{
//	luboparam lubopm = {};
//
//	CString strSubKey = RIGISTER_KEY + CString(_T("ProcParam"));
//
//	lubopm.tparam.procwidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("TPROCWIDTH")).operator LPCWSTR());
//	lubopm.tparam.procheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("TPROCHEIGHT")).operator LPCWSTR());
//	lubopm.tparam.showrate = std::stoi(getstrValueFromRegedit(strSubKey, _T("SHOWRATE")).operator LPCWSTR());
//	lubopm.tparam.isHchange = std::stoi(getstrValueFromRegedit(strSubKey, _T("ISHCHANGE")).operator LPCWSTR());
//	lubopm.tparam.roix = std::stoi(getstrValueFromRegedit(strSubKey, _T("TROIX")).operator LPCWSTR());
//	lubopm.tparam.roiy = std::stoi(getstrValueFromRegedit(strSubKey, _T("TROIY")).operator LPCWSTR());
//
//	lubopm.tparam.roiwidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("TROIWIDTH")).operator LPCWSTR());
//
//	lubopm.tparam.roiheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("TROIHEIGHT")).operator LPCWSTR());
//
//	lubopm.tparam.calibwidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("TCALIBWIDTH")).operator LPCWSTR());
//
//	lubopm.tparam.calibheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("TCALIBHEIGHT")).operator LPCWSTR());
//
//	lubopm.tparam.shield[0].x = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDX1")).operator LPCWSTR());
//
//	lubopm.tparam.shield[1].x = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDX2")).operator LPCWSTR());
//
//	lubopm.tparam.shield[2].x = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDX3")).operator LPCWSTR());
//
//	lubopm.tparam.shield[0].y = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDY1")).operator LPCWSTR());
//
//	lubopm.tparam.shield[1].y = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDY2")).operator LPCWSTR());
//
//	lubopm.tparam.shield[2].y = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDY3")).operator LPCWSTR());
//
//	lubopm.tparam.shield[0].width = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH1")).operator LPCWSTR());
//
//	lubopm.tparam.shield[1].width = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH2")).operator LPCWSTR());
//
//	lubopm.tparam.shield[2].width = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDWIDTH3")).operator LPCWSTR());
//
//	lubopm.tparam.shield[0].height = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT1")).operator LPCWSTR());
//
//	lubopm.tparam.shield[1].height = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT2")).operator LPCWSTR());
//
//	lubopm.tparam.shield[2].height = std::stoi(getstrValueFromRegedit(strSubKey, _T("TSHIELDHEIGHT3")).operator LPCWSTR());
//
//	lubopm.tparam.blackboardx = std::stoi(getstrValueFromRegedit(strSubKey, _T("BLACKBOARDX")).operator LPCWSTR());
//
//	lubopm.tparam.blackboardy = std::stoi(getstrValueFromRegedit(strSubKey, _T("BLACKBOARDY")).operator LPCWSTR());
//
//	lubopm.tparam.blackboardwidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("BLACKBOARDWIDTH")).operator LPCWSTR());
//
//	lubopm.tparam.blackboardheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("BLACKBOARDHEIGHT")).operator LPCWSTR());
//
//	lubopm.tparam.teacherdeskx = std::stoi(getstrValueFromRegedit(strSubKey, _T("TEACHERDESKX")).operator LPCWSTR());
//
//	lubopm.tparam.teacherdesky = std::stoi(getstrValueFromRegedit(strSubKey, _T("TEACHERDESKY")).operator LPCWSTR());
//
//	lubopm.tparam.teacherdeskwidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("TEACHERDESKWIDTH")).operator LPCWSTR());
//
//	lubopm.tparam.teacherdeskheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("TEACHERDESKHEIGHT")).operator LPCWSTR());
//
//	lubopm.tparam.zoomareax = std::stoi(getstrValueFromRegedit(strSubKey, _T("ZOOMAREAX")).operator LPCWSTR());
//
//	lubopm.tparam.zoomareay = std::stoi(getstrValueFromRegedit(strSubKey, _T("ZOOMAREAY")).operator LPCWSTR());
//	lubopm.tparam.zoomareawidth = std::stoi(getstrValueFromRegedit(strSubKey, _T("ZOOMAREAWIDTH")).operator LPCWSTR());
//	lubopm.tparam.zoomareaheight = std::stoi(getstrValueFromRegedit(strSubKey, _T("ZOOMAREAHEIGHT")).operator LPCWSTR());
//
//}

extern bool SetstrValueToRegedit(CString strSubKey, CString name, CString value);
void CameraThings::Save(EBBStatus eBBType, CRect(* rects)[4], CSize size)
{
	int calibwidth = m_imageOut.cols;
	int calibheight = m_imageOut.rows;
	CString strSubKey = RIGISTER_KEY + CString(_T("\\CloudClassroom\\ProcParam"));

	SetstrValueToRegedit(strSubKey, _T("CAMERA_IMG_WIDTH"), std::to_wstring(calibwidth).c_str());
	SetstrValueToRegedit(strSubKey, _T("CAMERA_IMG_HEIGHT"), std::to_wstring(calibheight).c_str());

	//需要保存的点
	for (int i = 0; i < eBBType; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CRect r = rects[i][j];
			int xDest = int((double)r.left / size.cx*calibwidth + 0.5);
			int yDest = int((double)r.top / size.cy*calibheight + 0.5);
			int wDest = int((double)(r.right - r.left) / size.cx*calibwidth + 0.5);
			int hDest = int((double)(r.bottom - r.top) / size.cy*calibheight + 0.5);
			std::wstring xKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"X";
			std::wstring yKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"Y";
			std::wstring wKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"W";
			std::wstring hKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"H";

			SetstrValueToRegedit(strSubKey, xKey.c_str(), std::to_wstring(xDest).c_str());
			SetstrValueToRegedit(strSubKey, yKey.c_str(), std::to_wstring(yDest).c_str());
			SetstrValueToRegedit(strSubKey, wKey.c_str(), std::to_wstring(wDest).c_str());
			SetstrValueToRegedit(strSubKey, hKey.c_str(), std::to_wstring(hDest).c_str());
		}
	}
}
int stoi(const std::wstring& s)
{
	if (s.empty())
	{
		return 0;
	}
	else
	{
		return std::stoi(s);
	}
}
extern CString getstrValueFromRegedit(LPCTSTR lpSubKey, LPCTSTR lpValueName);
CRect(*CameraThings::GetDataFromRegedit(EBBStatus eBBType, CSize size))[4]
{
	static CRect rects[4][4];
	memset(rects, 0, sizeof(rects));
	CString strSubKey = RIGISTER_KEY + CString(_T("\\CloudClassroom\\ProcParam"));
	int calibwidth = stoi(getstrValueFromRegedit(strSubKey, _T("CAMERA_IMG_WIDTH")).operator LPCWSTR());
	int calibheight = stoi(getstrValueFromRegedit(strSubKey, _T("CAMERA_IMG_HEIGHT")).operator LPCWSTR());
	
	//需要保存的点
	for (int i = 0; i < eBBType; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			CRect& r = rects[i][j];

			std::wstring xKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"X";
			std::wstring yKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"Y";
			std::wstring wKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"W";
			std::wstring hKey = std::to_wstring(eBBType) + L"BBAnchor" + std::to_wstring(i) + std::to_wstring(j) + L"H";

			int xDest = stoi(getstrValueFromRegedit(strSubKey, xKey.c_str()).operator LPCWSTR());
			int yDest = stoi(getstrValueFromRegedit(strSubKey, yKey.c_str()).operator LPCWSTR());
			int wDest = stoi(getstrValueFromRegedit(strSubKey, wKey.c_str()).operator LPCWSTR());
			int hDest = stoi(getstrValueFromRegedit(strSubKey, hKey.c_str()).operator LPCWSTR());

			if (calibwidth != 0 && calibheight != 0)
			{
				r.left = int((double)xDest / calibwidth *size.cx +0.5);
				r.top = int((double)yDest / calibheight*size.cy + 0.5);
				r.right = int((double)wDest / calibwidth*size.cx + 0.5) + r.left;
				r.bottom = int((double)hDest / calibheight *size.cy + 0.5) + r.top;
			}
			else
			{
				r = CRect();
			}
		}
	}
	return rects;
}

/*
luboparam lubopm = {};
if (m_roiRectt.width < 0)
{
m_roiRectt.width = abs(m_roiRectt.width);
m_roiRectt.x = m_roiRectt.x - m_roiRectt.width;
}
if (m_roiRectt.height < 0)
{
m_roiRectt.height = abs(m_roiRectt.height);
m_roiRectt.y = m_roiRectt.y - m_roiRectt.height;
}
lubopm.tparam.roix = m_roiRectt.x;
lubopm.tparam.roiy = m_roiRectt.y;
lubopm.tparam.roiwidth = m_roiRectt.width;
lubopm.tparam.roiheight = m_roiRectt.height;
lubopm.tparam.calibwidth = m_imageOut.cols;
lubopm.tparam.calibheight = m_imageOut.rows;


if (m_shieldRectt[0].width < 0)
{
m_shieldRectt[0].width = abs(m_shieldRectt[0].width);
m_shieldRectt[0].x = m_shieldRectt[0].x - m_shieldRectt[0].width;
}
if (m_shieldRectt[0].height < 0)
{
m_shieldRectt[0].height = abs(m_shieldRectt[0].height);
m_shieldRectt[0].y = m_shieldRectt[0].y - m_shieldRectt[0].height;
}
lubopm.tparam.shield[0].x = m_shieldRectt[0].x;
lubopm.tparam.shield[0].y = m_shieldRectt[0].y;
lubopm.tparam.shield[0].width = m_shieldRectt[0].width;
lubopm.tparam.shield[0].height = m_shieldRectt[0].height;

if (m_shieldRectt[1].width < 0)
{
m_shieldRectt[1].width = abs(m_shieldRectt[1].width);
m_shieldRectt[1].x = m_shieldRectt[1].x - m_shieldRectt[1].width;
}
if (m_shieldRectt[1].height < 0)
{
m_shieldRectt[1].height = abs(m_shieldRectt[1].height);
m_shieldRectt[1].y = m_shieldRectt[1].y - m_shieldRectt[1].height;
}
lubopm.tparam.shield[1].x = m_shieldRectt[1].x;
lubopm.tparam.shield[1].y = m_shieldRectt[1].y;
lubopm.tparam.shield[1].width = m_shieldRectt[1].width;
lubopm.tparam.shield[1].height = m_shieldRectt[1].height;

if (m_shieldRectt[2].width < 0)
{
m_shieldRectt[2].width = abs(m_shieldRectt[2].width);
m_shieldRectt[2].x = m_shieldRectt[2].x - m_shieldRectt[2].width;
}
if (m_shieldRectt[2].height < 0)
{
m_shieldRectt[2].height = abs(m_shieldRectt[2].height);
m_shieldRectt[2].y = m_shieldRectt[2].y - m_shieldRectt[2].height;
}
lubopm.tparam.shield[2].x = m_shieldRectt[2].x;
lubopm.tparam.shield[2].y = m_shieldRectt[2].y;
lubopm.tparam.shield[2].width = m_shieldRectt[2].width;
lubopm.tparam.shield[2].height = m_shieldRectt[2].height;


if (m_blackboardRect.width < 0)
{
m_blackboardRect.width = abs(m_blackboardRect.width);
m_blackboardRect.x = m_blackboardRect.x - m_blackboardRect.width;
}
if (m_blackboardRect.height < 0)
{
m_blackboardRect.height = abs(m_blackboardRect.height);
m_blackboardRect.y = m_blackboardRect.y - m_blackboardRect.height;
}
lubopm.tparam.blackboardx = m_blackboardRect.x;
lubopm.tparam.blackboardy = m_blackboardRect.y;
lubopm.tparam.blackboardwidth = m_blackboardRect.width;
lubopm.tparam.blackboardheight = m_blackboardRect.height;

if (m_teacherDeskRect.width < 0)
{
m_teacherDeskRect.width = abs(m_teacherDeskRect.width);
m_teacherDeskRect.x = m_teacherDeskRect.x - m_teacherDeskRect.width;
}
if (m_teacherDeskRect.height < 0)
{
m_teacherDeskRect.height = abs(m_teacherDeskRect.height);
m_teacherDeskRect.y = m_teacherDeskRect.y - m_teacherDeskRect.height;
}
lubopm.tparam.teacherdeskx = m_teacherDeskRect.x;
lubopm.tparam.teacherdesky = m_teacherDeskRect.y;
lubopm.tparam.teacherdeskwidth = m_teacherDeskRect.width;
lubopm.tparam.teacherdeskheight = m_teacherDeskRect.height;

m_zoomArea = m_roiRectt;

if (m_zoomArea.width < 0)
{
m_zoomArea.width = abs(m_zoomArea.width);
m_zoomArea.x = m_zoomArea.x - m_zoomArea.width;
}
if (m_zoomArea.height < 0)
{
m_zoomArea.height = abs(m_zoomArea.height);
m_zoomArea.y = m_zoomArea.y - m_zoomArea.height;
}
lubopm.tparam.zoomareax = m_zoomArea.x;
lubopm.tparam.zoomareay = m_zoomArea.y;
lubopm.tparam.zoomareawidth = m_zoomArea.width;
lubopm.tparam.zoomareaheight = m_zoomArea.height;

lubopm.tparam.showrate = m_showrate;
lubopm.tparam.speed = m_speed;

//计算黑板左上角矩形框的参数值
if (m_bboardlt.width < 0)
{
m_bboardlt.width = abs(m_bboardlt.width);
m_bboardlt.x = m_bboardlt.x - m_bboardlt.width;
}
if (m_bboardlt.height < 0)
{
m_bboardlt.height = abs(m_bboardlt.height);
m_bboardlt.y = m_bboardlt.y - m_bboardlt.height;
}
//将黑板左上角的参数值赋值给lubopm
lubopm.bparam.blackboardLT.x = m_bboardlt.x;
lubopm.bparam.blackboardLT.y = m_bboardlt.y;
lubopm.bparam.blackboardLT.width = m_bboardlt.width;
lubopm.bparam.blackboardLT.height = m_bboardlt.height;

//计算黑板右上角矩形框的参数值
if (m_bboardrt.width < 0)
{
m_bboardrt.width = abs(m_bboardrt.width);
m_bboardrt.x = m_bboardrt.x - m_bboardrt.width;
}
if (m_bboardrt.height < 0)
{
m_bboardrt.height = abs(m_bboardrt.height);
m_bboardrt.y = m_bboardrt.y - m_bboardrt.height;
}
//将黑板右上角的参数值赋值给lubopm
lubopm.bparam.blackboardRT.x = m_bboardrt.x;
lubopm.bparam.blackboardRT.y = m_bboardrt.y;
lubopm.bparam.blackboardRT.width = m_bboardrt.width;
lubopm.bparam.blackboardRT.height = m_bboardrt.height;

//计算黑板左下角矩形框的参数值
if (m_bboardlb.width < 0)
{
m_bboardlb.width = abs(m_bboardlb.width);
m_bboardlb.x = m_bboardlb.x - m_bboardlb.width;
}
if (m_bboardlb.height < 0)
{
m_bboardlb.height = abs(m_bboardlb.height);
m_bboardlb.y = m_bboardlb.y - m_bboardlb.height;
}
//将黑板左下角的参数值赋值给lubopm
lubopm.bparam.blackboardLB.x = m_bboardlb.x;
lubopm.bparam.blackboardLB.y = m_bboardlb.y;
lubopm.bparam.blackboardLB.width = m_bboardlb.width;
lubopm.bparam.blackboardLB.height = m_bboardlb.height;

//计算黑板右下角矩形框的参数值
if (m_bboardrb.width < 0)
{
m_bboardrb.width = abs(m_bboardrb.width);
m_bboardrb.x = m_bboardrb.x - m_bboardrb.width;
}
if (m_bboardrb.height < 0)
{
m_bboardrb.height = abs(m_bboardrb.height);
m_bboardrb.y = m_bboardrb.y - m_bboardrb.height;
}
//将黑板右下角的参数值赋值给lubopm
lubopm.bparam.blackboardRB.x = m_bboardrb.x;
lubopm.bparam.blackboardRB.y = m_bboardrb.y;
lubopm.bparam.blackboardRB.width = m_bboardrb.width;
lubopm.bparam.blackboardRB.height = m_bboardrb.height;



///////////////////////////////////////////////////////////////////
//计算黑板左上角矩形框的参数值
if (m_bboardlt2.width < 0)
{
m_bboardlt2.width = abs(m_bboardlt2.width);
m_bboardlt2.x = m_bboardlt2.x - m_bboardlt2.width;
}
if (m_bboardlt2.height < 0)
{
m_bboardlt2.height = abs(m_bboardlt2.height);
m_bboardlt2.y = m_bboardlt2.y - m_bboardlt2.height;
}
//将黑板左上角的参数值赋值给lubopm
lubopm.bparam2.blackboardLT.x = m_bboardlt2.x;
lubopm.bparam2.blackboardLT.y = m_bboardlt2.y;
lubopm.bparam2.blackboardLT.width = m_bboardlt2.width;
lubopm.bparam2.blackboardLT.height = m_bboardlt2.height;

//计算黑板右上角矩形框的参数值
if (m_bboardrt2.width < 0)
{
m_bboardrt2.width = abs(m_bboardrt2.width);
m_bboardrt2.x = m_bboardrt2.x - m_bboardrt2.width;
}
if (m_bboardrt2.height < 0)
{
m_bboardrt2.height = abs(m_bboardrt2.height);
m_bboardrt2.y = m_bboardrt2.y - m_bboardrt2.height;
}
//将黑板右上角的参数值赋值给lubopm
lubopm.bparam2.blackboardRT.x = m_bboardrt2.x;
lubopm.bparam2.blackboardRT.y = m_bboardrt2.y;
lubopm.bparam2.blackboardRT.width = m_bboardrt2.width;
lubopm.bparam2.blackboardRT.height = m_bboardrt2.height;

//计算黑板左下角矩形框的参数值
if (m_bboardlb2.width < 0)
{
m_bboardlb2.width = abs(m_bboardlb2.width);
m_bboardlb2.x = m_bboardlb2.x - m_bboardlb2.width;
}
if (m_bboardlb2.height < 0)
{
m_bboardlb2.height = abs(m_bboardlb2.height);
m_bboardlb2.y = m_bboardlb2.y - m_bboardlb2.height;
}
//将黑板左下角的参数值赋值给lubopm
lubopm.bparam2.blackboardLB.x = m_bboardlb2.x;
lubopm.bparam2.blackboardLB.y = m_bboardlb2.y;
lubopm.bparam2.blackboardLB.width = m_bboardlb2.width;
lubopm.bparam2.blackboardLB.height = m_bboardlb2.height;

//计算黑板右下角矩形框的参数值
if (m_bboardrb2.width < 0)
{
m_bboardrb2.width = abs(m_bboardrb2.width);
m_bboardrb2.x = m_bboardrb2.x - m_bboardrb2.width;
}
if (m_bboardrb2.height < 0)
{
m_bboardrb2.height = abs(m_bboardrb2.height);
m_bboardrb2.y = m_bboardrb2.y - m_bboardrb2.height;
}
//将黑板右下角的参数值赋值给lubopm
lubopm.bparam2.blackboardRB.x = m_bboardrb2.x;
lubopm.bparam2.blackboardRB.y = m_bboardrb2.y;
lubopm.bparam2.blackboardRB.width = m_bboardrb2.width;
lubopm.bparam2.blackboardRB.height = m_bboardrb2.height;

SetTchParamToDoc(lubopm);
*/