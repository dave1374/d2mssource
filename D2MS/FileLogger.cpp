
//Configurations:All Configurations
//
//Click on the Preprocessor Definitions line to invoke its editor
//
//Choose Edit...
//
//Copy "_CRT_SECURE_NO_WARNINGS" into the Preprocessor Definitions white box on the top.


#include "stdafx.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <ctime>

class FileLogger {

    public:

        enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO };
		
        explicit FileLogger (const char *fname)
                :   numWarnings (0U),
                    numErrors (0U)
        {			
			myFile.open( fname, std::ios::app ); 

            // Write the first lines
            if (myFile.is_open()) {
				time_t rawtime;
				time (&rawtime);

				
                myFile	<< std::endl 
						<< ctime (&rawtime)
						<< "Log file created " 
						<< std::endl 
						<< std::endl;
            } // if

        }


        // dtor
        ~FileLogger () {
            if (myFile.is_open()) {
                myFile << "=====================================================================";
                myFile.close();
            }
        }


        // Overload << operator using log type
        friend FileLogger &operator << (FileLogger &logger, const e_logType l_type) {

            switch (l_type) {
                case FileLogger::e_logType::LOG_ERROR:
                    logger.myFile << "[ERROR]: ";
                    ++logger.numErrors;
                    break;

                case FileLogger::e_logType::LOG_WARNING:
                    logger.myFile << "[WARNING]: ";
                    ++logger.numWarnings;
                    break;

                default:
                    logger.myFile << "[INFO]: ";
                    break;
            } 
            return logger;
        }

        // Overload << operator using C style strings
        // No need for std::string objects here
        friend FileLogger &operator << (FileLogger &logger, const char *text) {

            logger.myFile << text << std::endl;
            return logger;

        }


		friend FileLogger &operator << (FileLogger &logger, DWORD adr) {

			std::ostringstream ss;
			ss << std::hex << adr;

			logger.myFile << ss.str().c_str() << std::endl;
			return logger;

		}

		
    private:

        std::ofstream           myFile;
        unsigned int            numWarnings;
        unsigned int            numErrors;

}; // class end
