#include "player.h"

#define BUFFER_SIZE 	32

static uint8_t mp3Buffer[BUFFER_SIZE];
static uint32_t mp3FileSize;
static uint32_t readBytes;

bool isPlaying = false;
bool isFileOpen = false;
const char* mp3Path;

static FATFS fs;
static FIL mp3File;

static uint8_t mp3Volume = VOLUME_DEFAULT;

player_end_handle player_end_handler;
static void mp3_tim_task(void);

/* Initialize VS1053 & Open a file */
bool MP3_Init()
{
#if CONFIG_ENABLE_MP3
	if(!VS1053_Init()) return false;

	return true;
#endif
	return false;
}

bool MP3_Play(const char *filename)
{
	if(isPlaying) MP3_Stop();

	if(!VS1053_SetMode(0x4800)) return false;	/* SM LINE1 | SM SDINEW */
	if(!VS1053_AutoResync()) return false;		/* AutoResync */
	if(!VS1053_SetDecodeTime(0)) return false;	/* Set decode time */
	if(!VS1053_SetVolume(mp3Volume, mp3Volume)) return false;	/* Small number is louder */

	/* Open file to read */
	if(f_open(&mp3File, filename, FA_READ) != FR_OK) return false;

	/* Get the file size */
	mp3FileSize = f_size(&mp3File);
	
	MP3_Feeder();

	/* Set flags */
	isFileOpen = true;
	isPlaying = true;

  return true;
}

void MP3_Stop(void)
{
	/* Refer to page 49 of VS1053 datasheet */
	uint16_t mode;
	
	VS1053_SendEndFill(2052);	/* send endfill bytes */
	VS1053_SetMode(0x4808);		/* SM LINE1 | SM SDINEW | SM CANCEL */
	VS1053_SendEndFill(32);		/* send endfill bytes */
	
	VS1053_GetMode(&mode);		/* get mode value */
	if((mode & 0x08) != 0x0)	/* if SM CANCEL is not clear, soft reset */
	{
		VS1053_SoftReset();
	}

	f_close(&mp3File);
	isPlaying = false;			/* Stop flag */
	isFileOpen = false;			/* Close flag */
}

void MP3_Pause(void)
{
	if(isPlaying) isPlaying = false;
}

void MP3_Resume(void)
{
	if(!isPlaying) isPlaying = true;
}

/* Send mp3 buffer to VS1053 */
void MP3_Feeder(void)
{
	if(!isPlaying || !isFileOpen) return;

	/* Toggle Green LED */
	//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	
	f_read(&mp3File, mp3Buffer, 32, &readBytes);
	
	if(readBytes == 0)
	{
		MP3_Stop();
		return;
	}
	
	VS1053_SdiWrite32(mp3Buffer);
}

void MP3_SetVolume(uint8_t vol)
{
	if(vol <= VOLUME_MIN) vol = VOLUME_MIN;
	if(vol > VOLUME_MAX) vol = VOLUME_MAX;
	mp3Volume = (VOLUME_MIN + VOLUME_MAX) - vol; 
	Debug("vs10xxx volume: %d\n", mp3Volume);
	VS1053_SetVolume(mp3Volume, mp3Volume);
}

uint16_t MP3_GetSeconds()
{
	if(mp3Path == NULL) return 0;
	
	
}

