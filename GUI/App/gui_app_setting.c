#include "gui_app_setting.h"
#include "player.h"
#include "devices.h"

#define FONT_BUFF_SIZE 4096

#define TASK_FONT_INSTALL_PRIO				2
#define TASK_FONT_INSTALL_STACK_SIZE	1024
static TaskHandle_t TaskFontInstall_Handler; 
static void font_install_task(void *pvParameters);

typedef struct SettingObj
{
	lv_obj_t* cont_setting;
	lv_obj_t* btn_id;
	lv_obj_t* sw_language;
	lv_obj_t* sw_buzzer;
	lv_obj_t* sw_alarm;
	lv_obj_t* btn_font;
	lv_obj_t* btn_lcdbl;
	lv_obj_t* btn_lcdlock;
	lv_obj_t* btn_audio;
	lv_obj_t* btn_alarmlist;
	lv_obj_t* btn_reset;
	lv_obj_t* label_language;
	lv_obj_t* label_font;
	lv_obj_t* arc_load;
	lv_obj_t* arc_load_label;
	
}SettingObj_t;

enum
{
	ADJUST_BACKLIGHT,
	ADJUST_VOLUME,
	ADJUST_SCREEN_LOCK,
};

enum
{
	MSG_SETTING_RESET,
	MSG_FONT_INSTALL,
};

const char* titles[] = {
	"BackLight", "Volume", "Screen lock time(s)"
};

static SettingObj_t* lv_objs;
static OTS_t ots_install;

/********************************************************************/
static void font_install_task(void *pvParameters)
{
	FIL *file;
	FILINFO FileInf;
	FRESULT res;
	
	uint8_t *pbuff;
	uint32_t read_bytes;
	uint32_t nwrites = 0;
	uint32_t nblocks = 0;
	uint32_t addr = 0;
	bool install_flag = false;
	char prog_txt[16];
	
	//4个字节的偏移,用来保存写入标志//或者可以记录文件长度做标记??
	//这里只安装一个字库,就写入固定标志了
	uint8_t* flag = mymalloc(SRAMCCM, FONT_ZH_ADDR_OFFSET);
	if(flag != NULL) memset(flag, 0, FONT_ZH_ADDR_OFFSET);
	
	const char* path = pvParameters;
	Debug("font path: %s\n", path);
	
	do
	{
		file = mymalloc(SRAMIN,sizeof(FIL));
		pbuff = mymalloc(SRAMIN,FONT_BUFF_SIZE);
		if(file==NULL) break;
		
		res=f_open(file,path,FA_READ); //读取字库文件
		
		if(res!=FR_OK)	//打开文件失败
		{
			myfree(SRAMIN,file);
			myfree(SRAMIN,pbuff);
			break;
		}
		
		f_stat(path, &FileInf);
		Debug("Font file: %ld\n", FileInf.fsize);
		W25QXX_Write(flag, 0, FONT_ZH_ADDR_OFFSET);
		install_flag = true; //开始写入
	}while(0);
	
	while(install_flag)
	{
		if(f_eof(file) != 0) break;
		
		mymemset(pbuff, 0, FONT_BUFF_SIZE);
		f_read(file, pbuff, FONT_BUFF_SIZE, &read_bytes);
		
		W25QXX_Write(pbuff, (nblocks * FONT_BUFF_SIZE) + FONT_ZH_ADDR_OFFSET, read_bytes);
		nwrites+=read_bytes;
		nblocks++;
		
		sprintf(prog_txt, "%.2f%%", (nwrites * 1.0f / FileInf.fsize * 100.0f));
		//Debug("install progress: %s\n", prog_txt);
		lv_label_set_text(lv_objs->arc_load_label, prog_txt);
		vTaskDelay(2);
	}

	if(nwrites == FileInf.fsize)
	{
		//写入安装成功标志
		Debug("write install flag\n");
		W25QXX_Write((uint8_t*)(&font_install_flag), 0, FONT_ZH_ADDR_OFFSET);
		
		if(font_install_check())
		{
			Debug("font install success\n");
			device_cfg.font_zh_flag = true;
			device_config_write();
		}
		else
		{
			Debug("font install failed\n");
			device_cfg.font_zh_flag = false;
		}
		
	}

	f_close(file);
	myfree(SRAMIN,file);
	myfree(SRAMIN,pbuff);
	myfree(SRAMCCM, flag);
	
	vTaskDelay(1000);
	
	//删除加载进度框
	lv_obj_del(lv_objs->arc_load);
	//重启
	System_Reset();
	
	vTaskDelete(*ots_install.taskHandler);
}

/********************************************************************/
static void status_bar_hidecb()
{
	myfree(SRAMCCM, lv_objs);
	page_call_handler((void*)PAGE_APPS, DEL);
}

static void file_select_cb(const char* name, const char* path)
{	
	lv_objs->arc_load = lv_arc_load_create(page_self_root);
	lv_objs->arc_load_label = lv_arc_load_get_label(lv_objs->arc_load);
	
	lv_label_set_text(lv_objs->arc_load_label, "0%");
	
	
	ots_install.taskCode = font_install_task;
	ots_install.taskName = "font";
	ots_install.taskStackSize = TASK_FONT_INSTALL_STACK_SIZE;
	ots_install.taskParameters = (void*)path;
	ots_install.taskPriority = TASK_FONT_INSTALL_PRIO;
	ots_install.taskHandler = &TaskFontInstall_Handler;

	os_task_start(&ots_install);
}

static void file_close_cb()
{
	
}

static void adjust_slider_cb(uint8_t type, uint8_t val)
{
	Debug("slider: %d, %d\n", type, val);
	
	switch(type)
	{
		case ADJUST_BACKLIGHT:
			device_cfg.lcd_level = val;
			LCD_SetBlk(device_cfg.lcd_level);
			break;
		case ADJUST_VOLUME:
			device_cfg.vs10_volume = val;
			MP3_SetVolume(device_cfg.vs10_volume * 10);
			break;
		case ADJUST_SCREEN_LOCK:
			device_cfg.screen_close_delay = val;
			break;
	}
}

static void msgbox_event_cb(uint8_t index, uint8_t type)
{	
	switch(index)
	{
		case MSGBOX_OK:
			if(type == MSG_SETTING_RESET)
			{
				Debug("system reset\n");
				device_config_reset();
				System_Reset();
			}
			else if(type == MSG_FONT_INSTALL)
			{
				Debug("Reinstall font\n");
				font_install_flag_clear();
				device_cfg.font_zh_flag = font_install_check();
				device_config_write();
				
				System_Reset();
			}
			break;
		case MSGBOX_CANCEL:
			break;
	}
}

static void language_sw_set()
{
	lv_label_set_text_fmt(lv_objs->label_language, "Language: %s", device_cfg.language_flag ? "ZH" : "EN");
}

static void font_install_set()
{
	lv_label_set_text_fmt(lv_objs->label_font, "Font: %s", device_cfg.font_zh_flag ? "installed" : "not installed");
}

static void event_key_handler(lv_event_t *e)
{	
	switch(e->code)
	{
		case LV_EVENT_KEY:
		{
			const uint32_t key = lv_indev_get_key(lv_indev_get_act());
			lv_obj_t* obj = lv_event_get_target(e);
			
			if(key == LV_KEY_BACKSPACE)
			{
				status_bar->state_set(STATUS_BAR, DISABLE);
			}
			
		}	
			break;
	}
}

static void event_click_handler(lv_event_t* e)
{
	bool cfg_set = false;
	
	lv_obj_t* obj = lv_event_get_target(e);
	
	if(obj == lv_objs->btn_id)
	{
		Debug("device id: 0x%08X", device_cfg.device_id);
	}
	else if(obj == lv_objs->sw_language)
	{
		//语言切换开关
		device_cfg.language_flag = lv_obj_has_state(obj, LV_STATE_CHECKED);
		language_sw_set();
		cfg_set = true;
	}
	else if(obj == lv_objs->sw_buzzer)
	{
		//蜂鸣器开关
		device_cfg.sound_flag = lv_obj_has_state(obj, LV_STATE_CHECKED);
		status_bar->state_set(SOUND, device_cfg.sound_flag);
		cfg_set = true;
	}
	else if(obj == lv_objs->sw_alarm)
	{
		//闹钟开关
		device_cfg.alarm_flag = lv_obj_has_state(obj, LV_STATE_CHECKED);
		status_bar->state_set(ALARM, device_cfg.alarm_flag);
		cfg_set = true;
	}
	else if(obj == lv_objs->btn_font)
	{
		if(device_cfg.font_zh_flag)
		{
			//字库已安装,提示
			MsgParam_t param = {
				.title = "Font install",
				.info = "The font library has been installed. Reinstallation will clear the flag and restart",
				.msg_type = MSG_FONT_INSTALL,
				.re_focus_obj = lv_objs->cont_setting,
			};
			lv_mymsgbox_create(&param, msgbox_event_cb);
		}
		else
		{
			//字库未安装, 打开文件浏览
			file_manager->display(ENABLE);
		}
	}
	else if(obj == lv_objs->btn_lcdbl)
	{
		//LCD背光调节
		AdjustBarParam_t param = {
			.type = ADJUST_BACKLIGHT,
			.range_min = 1,
			.range_max = 10,
			.slide_gap = 1,
			.init_val = device_cfg.lcd_level,
			.re_focus_obj = lv_objs->cont_setting
		};
		
		param.title = titles[param.type];
		lv_adjust_slider_create(page_self_root, &param, adjust_slider_cb);
	}
	else if(obj == lv_objs->btn_lcdlock)
	{
		//LCD关闭延时
		AdjustBarParam_t param = {
			.type = ADJUST_SCREEN_LOCK,
			.range_min = 3,
			.range_max = 18,
			.slide_gap = 10,
			.init_val = device_cfg.screen_close_delay/10,
			.re_focus_obj = lv_objs->cont_setting
		};
		
		param.title = titles[param.type];
		lv_adjust_slider_create(page_self_root, &param, adjust_slider_cb);
	}
	else if(obj == lv_objs->btn_audio)
	{
		//VS1053音频声音调节
		AdjustBarParam_t param = {
			.type = ADJUST_VOLUME,
			.range_min = 3,
			.range_max = 13,
			.slide_gap = 1,
			.init_val = device_cfg.vs10_volume,
			.re_focus_obj = lv_objs->cont_setting
		};
		
		param.title = titles[param.type];
		lv_adjust_slider_create(page_self_root, &param, adjust_slider_cb);
	}
	else if(obj == lv_objs->btn_alarmlist)
	{
		//闹钟列表设置
		lv_alarm_list_create(page_self_root, lv_objs->cont_setting);
	}
	else if(obj == lv_objs->btn_reset)
	{
		//恢复默认设置
		MsgParam_t param = {
			.title = "Reset config",
			.info = "Reset configuration requires restart!",
			.msg_type = MSG_SETTING_RESET,
			.re_focus_obj = lv_objs->cont_setting,
		};
		lv_mymsgbox_create(&param, msgbox_event_cb);
	}
	
	if(cfg_set)
	{
		device_config_write();
	}
	
}

void gui_setting_init(lv_obj_t* root)
{
	page_self_root = root;
	
	lv_objs = mymalloc(SRAMCCM, sizeof(SettingObj_t));
	if(lv_objs != NULL) memset(lv_objs, 0, sizeof(SettingObj_t));
	
	lv_obj_t* label;

	lv_objs->cont_setting = lv_obj_create(page_self_root);
	lv_obj_set_size(lv_objs->cont_setting, LV_PCT(98), 200);
	lv_gridnav_add(lv_objs->cont_setting, LV_GRIDNAV_CTRL_ROLLOVER);
	lv_group_add_obj(lv_group_get_default(), lv_objs->cont_setting);
	lv_obj_align(lv_objs->cont_setting, LV_ALIGN_TOP_MID, 0, 35);
	lv_obj_set_style_bg_opa(lv_objs->cont_setting, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_DEFAULT); //禁止显示滚动条
	lv_obj_set_style_bg_opa(lv_objs->cont_setting, LV_OPA_0, LV_PART_SCROLLBAR | LV_STATE_SCROLLED);
	lv_obj_add_event_cb(lv_objs->cont_setting, event_key_handler, LV_EVENT_KEY, NULL);

	//设备ID
	lv_obj_t* label_id = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_id, LV_PCT(100));
	lv_obj_set_style_pad_top(label_id, 10, LV_PART_MAIN);
	lv_obj_set_style_pad_bottom(label_id, 10, LV_PART_MAIN);
	lv_obj_set_style_text_color(label_id, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text_fmt(label_id, "Device ID: 0x%08X", MY_DEVICE_ID);
	lv_obj_align(label_id, LV_ALIGN_TOP_LEFT, 0, 0);

	lv_objs->btn_id = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_id, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_id, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_id, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_id, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(btn, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_id, label_id, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_id);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);

	//语言
	lv_objs->label_language = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(lv_objs->label_language, LV_PCT(100));
	lv_obj_set_style_text_color(lv_objs->label_language, lv_color_black(), LV_PART_MAIN);
	lv_obj_align_to(lv_objs->label_language, label_id, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
	language_sw_set(); //设置文本提示

	lv_objs->sw_language = lv_switch_create(lv_objs->cont_setting);
	if(device_cfg.language_flag) lv_obj_add_state(lv_objs->sw_language, LV_STATE_CHECKED); //读取配置初始化
	lv_obj_add_event_cb(lv_objs->sw_language, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->sw_language, 40, 20);
	lv_obj_align_to(lv_objs->sw_language, lv_objs->label_language, LV_ALIGN_RIGHT_MID, 0, 0);

	//蜂鸣器
	lv_obj_t* label_buzzer = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_buzzer, LV_PCT(100));
	lv_obj_set_style_text_color(label_buzzer, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_buzzer, "Buzzer:");
	lv_obj_align_to(label_buzzer, lv_objs->label_language, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->sw_buzzer = lv_switch_create(lv_objs->cont_setting);
	if(device_cfg.sound_flag) lv_obj_add_state(lv_objs->sw_buzzer, LV_STATE_CHECKED); //读取配置初始化
	lv_obj_add_event_cb(lv_objs->sw_buzzer, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->sw_buzzer, 40, 20);
	lv_obj_align_to(lv_objs->sw_buzzer, label_buzzer, LV_ALIGN_RIGHT_MID, 0, 0);

	//闹钟
	lv_obj_t* label_alarm = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_alarm, LV_PCT(100));
	lv_obj_set_style_text_color(label_alarm, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_alarm, "Alarm:");
	lv_obj_align_to(label_alarm, label_buzzer, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->sw_alarm = lv_switch_create(lv_objs->cont_setting);
	if(device_cfg.alarm_flag) lv_obj_add_state(lv_objs->sw_alarm, LV_STATE_CHECKED); //读取配置初始化
	lv_obj_add_event_cb(lv_objs->sw_alarm, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->sw_alarm, 40, 20);
	lv_obj_align_to(lv_objs->sw_alarm, label_alarm, LV_ALIGN_RIGHT_MID, 0, 0);

	//字库
	lv_objs->label_font = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(lv_objs->label_font, LV_PCT(100));
	lv_obj_set_style_text_color(lv_objs->label_font, lv_color_black(), LV_PART_MAIN);
	lv_obj_align_to(lv_objs->label_font, label_alarm, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
	font_install_set(); //设置文本提示

	lv_objs->btn_font = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_font, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_font, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_font, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_font, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_font, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_font, lv_objs->label_font, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_font);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);

	//背光
	lv_obj_t* label_bl = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_bl, LV_PCT(100));
	lv_obj_set_style_text_color(label_bl, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_bl, "LCD Backlight: ");
	lv_obj_align_to(label_bl, lv_objs->label_font, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->btn_lcdbl = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_lcdbl, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_lcdbl, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_lcdbl, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_lcdbl, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_lcdbl, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_lcdbl, label_bl, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_lcdbl);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);
	
	//屏幕关闭延时
	lv_obj_t* label_slock = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_slock, LV_PCT(100));
	lv_obj_set_style_text_color(label_slock, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_slock, "Lock screen: ");
	lv_obj_align_to(label_slock, label_bl, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->btn_lcdlock = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_lcdlock, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_lcdlock, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_lcdlock, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_lcdlock, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_lcdlock, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_lcdlock, label_slock, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_lcdlock);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);

	//VS1053声音
	lv_obj_t* label_audio = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_audio, LV_PCT(100));
	lv_obj_set_style_text_color(label_audio, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_audio, "VS10 Audio: ");
	lv_obj_align_to(label_audio, label_slock, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->btn_audio = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_audio, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_audio, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_audio, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_audio, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_audio, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_audio, label_audio, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_audio);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);

	//闹钟
	lv_obj_t* label_alarm_list = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_alarm_list, LV_PCT(100));
	lv_obj_set_style_text_color(label_alarm_list, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_alarm_list, "Alarm Set: ");
	lv_obj_align_to(label_alarm_list, label_audio, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->btn_alarmlist = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_alarmlist, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_alarmlist, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_alarmlist, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_alarmlist, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_alarmlist, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_alarmlist, label_alarm_list, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_alarmlist);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_RIGHT);
	lv_obj_center(label);

	//重置默认值
	lv_obj_t* label_reset = lv_label_create(lv_objs->cont_setting);
	lv_obj_set_width(label_reset, LV_PCT(100));
	lv_obj_set_style_text_color(label_reset, lv_color_black(), LV_PART_MAIN);
	lv_label_set_text(label_reset, "Setting Reset: ");
	lv_obj_align_to(label_reset, label_alarm_list, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);

	lv_objs->btn_reset = lv_btn_create(lv_objs->cont_setting);
	lv_obj_add_event_cb(lv_objs->btn_reset, event_click_handler, LV_EVENT_CLICKED, NULL);
	lv_obj_set_size(lv_objs->btn_reset, 40, 20);
	lv_obj_set_style_bg_color(lv_objs->btn_reset, lv_color_white(), LV_PART_MAIN);
	lv_obj_set_style_shadow_opa(lv_objs->btn_reset, LV_OPA_0, LV_PART_MAIN);
	//lv_obj_set_style_border_opa(lv_objs->btn_reset, LV_OPA_0, LV_PART_MAIN);
	lv_obj_align_to(lv_objs->btn_reset, label_reset, LV_ALIGN_RIGHT_MID, 0, 0);

	label = lv_label_create(lv_objs->btn_reset);
	lv_obj_set_style_text_color(label, lv_palette_main(LV_PALETTE_BLUE), LV_PART_MAIN);
	lv_label_set_text(label, LV_SYMBOL_REFRESH);
	lv_obj_center(label);
}

void gui_setting_focus(void)
{
	lv_group_focus_obj(lv_objs->cont_setting);
	
	file_manager->select_cb = file_select_cb;
	file_manager->close_cb = file_close_cb;
	file_manager->display(DISABLE);
	file_manager->select_once = true;
	file_manager->pfilter = ".BIN"; 
	file_manager->re_focus_obj = lv_objs->cont_setting;
	
	status_bar->state_set(STATUS_BAR, ENABLE);
	status_bar->state_set(TIME, ENABLE);
	status_bar->hide_cb = status_bar_hidecb;
	status_bar->title_set(app_names[PAGE_APP_SETTING - 3]);
}



