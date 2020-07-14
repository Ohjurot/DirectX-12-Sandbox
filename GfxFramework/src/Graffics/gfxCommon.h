#pragma once

#define COM_RELEASE(ptrCom) if(ptrCom) {ptrCom->Release(); ptrCom = NULL;}
#define COM_SAFE_CREATE_HR_RETURN(hrTemp, exp) if(FAILED(hrTemp = exp)){Shutdown(); return hrTemp;}
#define COM_CREATE_HR_RETURN(hrTemp, exp) if(FAILED(hrTemp = exp)){return hrTemp;}