#include "HID.h"
#include "EasyCon_API.h"
#include "usb_pwr.h"
#include "led.h"

USB_JoystickReport_Input_t next_report;

// Reset report to default.
void ResetReport(void)
{
  memset((void *)&next_report, 0, sizeof(USB_JoystickReport_Input_t));
  next_report.LX = STICK_CENTER;
  next_report.LY = STICK_CENTER;
  next_report.RX = STICK_CENTER;
  next_report.RY = STICK_CENTER;
  next_report.HAT = HAT_CENTER;
}
void SetButtons(const uint16_t Button) {next_report.Button = Button;}
void PressButtons(const uint16_t Button) {next_report.Button |= Button;}
void ReleaseButtons(const uint16_t Button) {next_report.Button &= ~(Button);}
void SetHATSwitch(const uint8_t HAT) {next_report.HAT = HAT;}
void SetLeftStick(const uint8_t LX, const uint8_t LY)
{
  next_report.LX = LX; next_report.LY = LY;
}
void SetRightStick(const uint8_t RX, const uint8_t RY)
{
  next_report.RX = RX; next_report.RY = RY;
}
extern  __IO uint32_t bDeviceState; /* USB device status */
int usbd_send(uint8_t *usbd_send_buf, char *TAG)
{
	if( bDeviceState != CONFIGURED )
		return 1;
	ledb_on();
	memcpy(HIDTxBuffer,&next_report,sizeof(next_report));

	if(USBD_ENDPx_DataUp( ENDP1, HIDTxBuffer, sizeof(next_report) ) == NoREADY)	
	{
		return 1;
	}
	ledb_off();
	return 0;
}

void HIDInit(void)
{
  ResetReport();
	usbd_send((uint8_t *)&next_report, NULL);
}

void Report_Task(void)
{
	if(EasyCon_need_send_report())
	{
		usbd_send((uint8_t *)&next_report, NULL);
		EasyCon_report_send_callback();
	}
}

void HIDTask(void)
{
  // We need to run our task to process and deliver data for our IN and OUT endpoints.
  Report_Task();
}
