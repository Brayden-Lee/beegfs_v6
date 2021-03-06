#include "StringTk.h"
#include "StorageTk.h"
#include "ICommonConfig.h"
#include "LogContext.h"

/**
 * Loads a file into a string list (line by line).
 *
 * Loaded strings are trimmed before they are added to the list. Empty lines  and lines starting
 * with STORAGETK_FILE_COMMENT_CHAR are not added to the list.
 */
bool ICommonConfig::loadStringListFile(const char* filename, StringList& outList)
{
   const char* logContext = "Load file into string list";
   char line[STORAGETK_FILE_MAX_LINE_LENGTH];
   
   std::ifstream fis(filename);
   if(!fis.is_open() || fis.fail() )
   {
      LogContext(logContext).logErr(std::string("Failed to open file: ") + filename);
      return false;
   }
   
   while(!fis.eof() && !fis.fail() )
   {
      fis.getline(line, STORAGETK_FILE_MAX_LINE_LENGTH);
      std::string trimmedLine = StringTk::trim(line);
      if(trimmedLine.length() && (trimmedLine[0] != STORAGETK_FILE_COMMENT_CHAR) )
         outList.push_back(trimmedLine);
   }
   
   fis.close();

   return true;
}





