#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* https://blog.csdn.net/u010011236/article/details/53026127 */

/**
 * Convert PCM16LE raw data to WAVE format
 * @param pcmpath       Input PCM file.
 * @param channels      Channel number of PCM file.
 * @param sample_rate   Sample rate of PCM file.
 * @param wavepath      Output WAVE file.
 */
int simplest_pcm16le_to_wave(const char *pcmpath, int sample_rate, int channels, const char *wavepath)
{
    typedef struct WAVE_HEADER{
        char    fccID[4];       //����Ϊ""RIFF
        unsigned long dwSize;   //�����д��WAVE��ʽ��Ƶ�Ĵ�С
        char    fccType[4];     //����Ϊ"WAVE"
    }WAVE_HEADER;

    typedef struct WAVE_FMT{
        char    fccID[4];          //����Ϊ"fmt "
        unsigned long  dwSize;     //����ΪWAVE_FMTռ���ֽ�����Ϊ16
        unsigned short wFormatTag; //���ΪPCM����ֵΪ 1
        unsigned short wChannels;  //ͨ��������ͨ��=1��˫ͨ��=2
        unsigned long  dwSamplesPerSec;//����Ƶ��
        unsigned long  dwAvgBytesPerSec;/* ==dwSamplesPerSec*wChannels*uiBitsPerSample/8 */
        unsigned short wBlockAlign;//==wChannels*uiBitsPerSample/8
        unsigned short uiBitsPerSample;//ÿ���������bit����8bits=8, 16bits=16
    }WAVE_FMT;

    typedef struct WAVE_DATA{
        char    fccID[4];       //����Ϊ"data"
        unsigned long dwSize;   //==NumSamples*wChannels*uiBitsPerSample/8
    }WAVE_DATA;

#if 0
    if(channels==2 || sample_rate==0)
    {
        channels = 2;
        sample_rate = 44100;
    }
#endif	
    int bits = 16;

    WAVE_HEADER pcmHEADER;
    WAVE_FMT    pcmFMT;
    WAVE_DATA   pcmDATA;

    unsigned short m_pcmData;
    FILE *fp, *fpout;

    fp = fopen(pcmpath, "rb+");
    if(fp==NULL)
    {
        printf("Open pcm file error.\n");
        return -1;
    }
    fpout = fopen(wavepath, "wb+");
    if(fpout==NULL)
    {
        printf("Create wav file error.\n");
        return -1;
    }

    /* WAVE_HEADER */
    memcpy(pcmHEADER.fccID, "RIFF", strlen("RIFF"));
    memcpy(pcmHEADER.fccType, "WAVE", strlen("WAVE"));
    fseek(fpout, sizeof(WAVE_HEADER), 1);   //1=SEEK_CUR
    /* WAVE_FMT */
    memcpy(pcmFMT.fccID, "fmt ", strlen("fmt "));
    pcmFMT.dwSize = 16;
    pcmFMT.wFormatTag = 1;
    pcmFMT.wChannels = channels;
    pcmFMT.dwSamplesPerSec = sample_rate;
    pcmFMT.uiBitsPerSample = bits;
    /* ==dwSamplesPerSec*wChannels*uiBitsPerSample/8 */
    pcmFMT.dwAvgBytesPerSec = pcmFMT.dwSamplesPerSec*pcmFMT.wChannels*pcmFMT.uiBitsPerSample/8;
    /* ==wChannels*uiBitsPerSample/8 */
    pcmFMT.wBlockAlign = pcmFMT.wChannels*pcmFMT.uiBitsPerSample/8;


    fwrite(&pcmFMT, sizeof(WAVE_FMT), 1, fpout);

    /* WAVE_DATA */
    memcpy(pcmDATA.fccID, "data", strlen("data"));
    pcmDATA.dwSize = 0;
    fseek(fpout, sizeof(WAVE_DATA), SEEK_CUR);

    fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    while(!feof(fp))
    {
        pcmDATA.dwSize += 2;
        fwrite(&m_pcmData, sizeof(unsigned short), 1, fpout);
        fread(&m_pcmData, sizeof(unsigned short), 1, fp);
    }

    /*pcmHEADER.dwSize = 44 + pcmDATA.dwSize;*/
    //�޸�ʱ�䣺2018��1��5��
    pcmHEADER.dwSize = 36 + pcmDATA.dwSize;

    rewind(fpout);
    fwrite(&pcmHEADER, sizeof(WAVE_HEADER), 1, fpout);
    fseek(fpout, sizeof(WAVE_FMT), SEEK_CUR);
    fwrite(&pcmDATA, sizeof(WAVE_DATA), 1, fpout);

    fclose(fp);
    fclose(fpout);

    return 0;
}

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		printf("Usage:\n");
		printf("%s <input pcm file> <sample_rate> <channels>  <output wav file>\n", argv[0]);
		return -1;
	}
	
    simplest_pcm16le_to_wave(argv[1], strtol(argv[2], NULL, 0), strtol(argv[3], NULL, 0), argv[4]);

    return 0;
}
