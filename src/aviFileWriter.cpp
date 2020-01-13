

//=============================================================================
// System Includes
//=============================================================================
#include <cassert>
#include <cstdio>
#include <limits.h>
#include <iostream>

//=============================================================================
// Project Includes
//=============================================================================
#include "aviFileWriter.h"
//#include "helper.h"
#include <opencv2/videoio/videoio_c.h>


using namespace::cv;
using namespace::std;
//
// Extra temp data bytes.
//
#define EXTRA_TEMP_SPACE 10000

AviFileWriter::AviFileWriter()
{
  // m_pavifile		= NULL;
  // m_pstream		= NULL;
  // m_pstreamcompressed	= NULL;
   m_pTempBMPBuffer	= NULL;
   m_pTempBuffer	= NULL;
  // m_pBitmapInfo        = NULL;

   m_iTimeIndex	     = 0;

   m_iCols     = 0;
   m_iRows     = 0;
   m_iBPP      = 0;
   m_iSize     = 0;
   m_iRowInc   = 0;
   m_frameRate = 0.0;

   m_bSizeLimited = false;
   m_iSplitFile = 0;
   m_isColor = true;

   // Default compression options.
   //m_compvars.fccHandler = mmioFOURCC( 'D', 'I', 'B', ' ' ); // uncompressed
   //m_compvars.fccHandler = mmioFOURCC( 'I', 'V', '5', '0' );
   //m_compvars.dwFlags    = 0x0;
   //m_compvars.lQ         = ICQUALITY_DEFAULT;

   //m_compvars.lpState = NULL;

   m_liBytesWritten = 0;
}


AviFileWriter::~AviFileWriter()
{
   close();

   /*if( m_pBitmapInfo != NULL )
   {
      free( m_pBitmapInfo );
      m_pBitmapInfo = NULL;
   }*/

   if( m_pTempBuffer != NULL )
   {
      delete [] m_pTempBuffer;
      m_pTempBuffer = NULL;
   }

   if( m_pTempBMPBuffer != NULL )
   {
      delete [] m_pTempBMPBuffer;
      m_pTempBMPBuffer = NULL;
   }

   /*if( m_compvars.lpState != NULL )
   {
      delete [] (char *) m_compvars.lpState; // (char *)  nur um warnung zu unterdruecken
      m_compvars.lpState = NULL;
   }*/
}


bool AviFileWriter::openSizeLimitedAVI(
                 const char*  pszFilename,
                 int	      iCols,
                 int	      iRows,
                 int	      ibpp,
                 double	      dFramerate )
{
#ifdef WINDOWS
   char szAVIFile[ _MAX_PATH ];
#else
   char szAVIFile[ PATH_MAX ];
#endif
   m_bSizeLimited = true;
   m_iSplitFile = 0;

   sprintf(szAVIFile, "%s", pszFilename );

   // Remove any extensions.
   char* pcExt = strstr( szAVIFile, "." );
   if( pcExt != NULL )
   {
      *( pcExt ) = 0x0;
   }

   //
   // Keep the basic avi file name in m_szAVIDestFile.
   //
   sprintf(m_szAVIDestFile, "%s", szAVIFile );

   //
   // The avi file names will be in the form  of ***_0000.avi, ***_0001.avi,
   // ***_0002.avi...
   // The size of each avi file is limited by AVI_FILE_SPLIT_SIZE bytes
   //
   sprintf(szAVIFile, "%s_%04d.avi", m_szAVIDestFile, m_iSplitFile );
   return open( szAVIFile, iCols, iRows, ibpp, dFramerate );
}

bool AviFileWriter::open(const char*  pszFilename, int iCols, int iRows, int ibpp, double dFramerate)
{
   //
   // Initialize the library.
   //
   //::AVIFileInit();

   if( !checkOpenCVVersion() )
   {
      return false;
   }

   if(pszFilename == NULL || iRows <= 0 || iCols <= 0 || dFramerate <= 0.0)
   {
      assert( false );
      return false;
   }

   /*if(ibpp != 24 && ibpp != 32 && ibpp != 8 && ibpp != 16)
   {
      assert( false );
      return false;
   }*/

   m_frameRate = dFramerate;
   m_iTimeIndex	= 0;
   m_liBytesWritten = 0;
   //
   // If these parameters change, set them and allocate the buffers
   //
   if (m_iCols != iCols || m_iRows != iRows || m_iBPP  != ibpp)
   {
      //
      // Initialize members.
      //
      m_iCols     = iCols;
      m_iRows     = iRows;
      m_iBPP      = ibpp;
      m_iRowInc   = m_iCols * ( m_iBPP / 8 );
      m_iSize     = m_iRows * m_iRowInc;
      //
      // Allocate temp image buffers.
      //
      if(m_pTempBMPBuffer != NULL)
      {
         delete [] m_pTempBMPBuffer;
         m_pTempBMPBuffer = NULL;
      }
      m_pTempBMPBuffer = new unsigned char[m_iSize + EXTRA_TEMP_SPACE];

      if(m_pTempBuffer != NULL)
      {
         delete [] m_pTempBuffer;
         m_pTempBuffer = NULL;
      }
      m_pTempBuffer = new unsigned char[ m_iSize ];
   }

   if (m_pTempBMPBuffer == NULL || m_pTempBuffer == NULL)
      return false;

   //
   // If this function is called from openSizeLimitedAVI(), pszFilename
   // is the file to open. m_szAVIDestFile should not be changed because
   // it is the base file name used in appendFrame() to generate the next
   // split file name.
   //
   // If this function is called directly (not by openSizeLimitedAVI()),
   // the AVI file can not be splited and m_szAVIDestFile is not used.
   // So we do not need to save the file name to m_szAVIDestFile.
   //
#if 0
   sprintf(m_szAVIDestFile, "%s", pszFilename );

   // Remove any extensions.
   char* pcExt = strstr( m_szAVIDestFile, "." );
   if( pcExt != NULL )
   {
      *( pcExt ) = 0x0;
   }

   // add an avi extension so that the file can be opened
   char szAVIFile[ _MAX_PATH ];
   sprintf(szAVIFile, "%s.avi", m_szAVIDestFile);
#endif

   //
   // Open the file.
   //
   //char *c = "Ein C-String";
   //WCHAR wc[13];
   //for (int i = 0; i < 13; i++){
   //    wc[i] = (WCHAR) c[i];
   //}

//#ifdef UNICODE
//   printf("UNICODE\n");
//#else
//      printf("NO UNICODE\n");
//#endif
   //m_vWriter.open(pszFilename, CV_FOURCC_DEFAULT, dFramerate, Size(iCols,iRows), m_isColor);
   return m_vWriter.open(pszFilename, CV_FOURCC_DEFAULT/*PROMPT*/, dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename, CV_FOURCC('H', 'F', 'Y', 'U'), dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename, CV_FOURCC('X', 'V', 'I', 'D'), dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename, CV_FOURCC('I', '4', '2', '0'), dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename, CV_FOURCC('X', '2', '6', '4'), dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename, CV_FOURCC('P', 'I', 'M', '1'), dFramerate, Size(iCols,iRows), m_isColor);
   //m_vWriter.open(pszFilename,CV_FOURCC('M', 'J', 'P', 'G'),dFramerate,Size(iCols,iRows),m_isColor);
   //m_vWriter.open(pszFilename,CV_FOURCC('I', 'V', '5', '0'),dFramerate,Size(iCols,iRows),m_isColor);
   //HRESULT hr = ::AVIFileOpen(&m_pavifile, pszFilename, OF_WRITE | OF_CREATE, NULL); //(WCHAR*) pszFilename L"mbhuhu.avi"


   //return true;
}


bool AviFileWriter::open(
                 const char*  pszFilename,
                 int	      iCols,
                 int	      iRows,
                 int	      ibpp,
                 int	      iFramerate )
{
  //return m_vWriter.open(pszFilename,CV_FOURCC_DEFAULT,(double) iFramerate,Size(iCols,iRows),m_isColor);
    return m_vWriter.open(pszFilename,CV_FOURCC_PROMPT,(double) iFramerate,Size(iCols,iRows),m_isColor);
  //return m_vWriter.open(pszFilename,CV_FOURCC('H', 'F', 'Y', 'U'),(double) iFramerate,Size(iCols,iRows),m_isColor);
  //return m_vWriter.open(pszFilename,CV_FOURCC('X', 'V', 'I', 'D'),(double) iFramerate,Size(iCols,iRows),m_isColor);
  //return m_vWriter.open(pszFilename,CV_FOURCC('I', '4', '2', '0'),(double) iFramerate,Size(iCols,iRows),m_isColor);
  //return m_vWriter.open(pszFilename,CV_FOURCC('X', '2', '6', '4'),(double) iFramerate,Size(iCols,iRows),m_isColor);
  //return m_vWriter.open(pszFilename,CV_FOURCC('P', 'I', 'M', '1'),(double) iFramerate,Size(iCols,iRows),m_isColor);
}

long int AviFileWriter::bytesWritten()
{
   return m_liBytesWritten;
   //return m_iTimeIndex * m_iSize;
}

bool AviFileWriter::appendFrame(const unsigned char* pBuffer, bool bInvert)
{
   if( !m_vWriter.isOpened() ){
         assert(false);
         return false;
   }


   unsigned char* pWriteBuffer = (unsigned char*) pBuffer;
   Mat frame;

//   cout << "m_iRowInc: " << m_iRowInc << endl;
//   cout << "rows: " << m_iRows << " cols: " << m_iCols << endl;
//   cout << m_iRowInc / m_iCols << endl;

   if( (m_iRowInc / m_iCols) == 1 )
   {
       frame = Mat(m_iRows, m_iCols, CV_8UC1, pWriteBuffer, m_iRowInc);// = imread(pszFilename);
   }else if ( (m_iRowInc / m_iCols) == 3 )
   {
       frame = Mat(m_iRows, m_iCols, CV_8UC3, pWriteBuffer, m_iRowInc);// = imread(pszFilename);
   }else if ( (m_iRowInc / m_iCols) == 4 )
   {
       frame = Mat(m_iRows, m_iCols, CV_8UC4, pWriteBuffer, m_iRowInc);// = imread(pszFilename);
       cvtColor(frame,frame,COLOR_RGBA2RGB); // need for right image interpretation
   }else
   {
       cout << "error: unkown video format." << endl;
       return false;
   }
   if( !frame.data )
   {
       cout << "error: while saving video data." << endl;
       return false;
   }


   //frame.create(m_vWriter.get(CV_CAP_PROP_FRAME_WIDTH),m_vWriter.get(CV_CAP_PROP_FRAME_HEIGHT),CV_8UC3);
   //frame.data = (unsigned char*) pBuffer;

   //const unsigned char* pWriteBuffer = pBuffer;

//   namedWindow("TEST");
//   imshow("TEST",frame);
//   waitKey(1);

//   cout << "Frame size: " << frame.rows << "x" << frame.cols << endl;
//   cout << "Videowriter size:" << m_iRows << "x" << m_iCols << endl;

//   m_vWriter.write(frame.clone());
   m_vWriter.write(frame);

   /**
        * ToDo: invertieren
        *
        */

   //LONG lSamplesWritten;
   //LONG lBytesWritten;
   long lBytesWritten;

   //
   // If the AVI file is opened with openSizeLimitedAVI(), split it if necessory.
   //
   if (m_bSizeLimited)
   {
      //
      // Split the file if it is more than 'AVI_FILE_SPLIT_SIZE' bytes
      //
      //if(bytesWritten() >= (__int64)(AVI_FILE_SPLIT_SIZE))
      if(bytesWritten() >= (long int)(AVI_FILE_SPLIT_SIZE))
      {
#ifdef WINDOWS
         char szAVIFile[_MAX_PATH];
#else
          char szAVIFile[PATH_MAX];
#endif
         close();
         m_iSplitFile++;
         sprintf(szAVIFile, "%s_%04d.avi", m_szAVIDestFile, m_iSplitFile );
         if( !open( szAVIFile, m_iCols, m_iRows, m_iBPP, m_frameRate ) )
            return false;
      }
   }

   m_liBytesWritten += lBytesWritten;

   m_iTimeIndex++;

   return true;
}

bool AviFileWriter::close()
{
   m_vWriter.release();
   return true;
}


bool AviFileWriter::appendBMP( const char* pszFilename )
{

   Mat bmpFrame = imread(pszFilename);

   m_vWriter.write(bmpFrame);

   m_iTimeIndex++;

   return true;
}


bool AviFileWriter::checkOpenCVVersion()
{
   //std::cout << "Established OpenCV Version: " << CV_VERSION << std::endl;
   //cout << "Version: " << CV_VERSION
        //	<< ", Major Version: " << CV_MAJOR_VERSION
        //	<< ", Minor Version: " << CV_MINOR_VERSION
        //	<< ", Subminor Version: " << CV_SUBMINOR_VERSION
        //	<< endl;

   //DWORD dwVer = ::VideoForWindowsVersion();

   if( CV_MAJOR_VERSION < 3 )
   {
      assert( false );  // opencv version too old.
      return false;
   }
   //else
   //      if( CV_MINOR_VERSION < 1 )
   //{
   //       assert( false );
   //       return false;
   //}

   return true;
}


