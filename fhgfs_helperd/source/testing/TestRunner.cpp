#include "TestRunner.h"

#include "TestConfig.h"
#include "TestCommunication.h"
#include <program/Program.h>

/*
 * @param outputFormat the output format of the tests (text,xml,...)
 *
 * no output file is given, so output goes to stderr
 */
TestRunner::TestRunner(TestRunnerOutputFormat outputFormat) :
   TestRunnerBase(outputFormat)
{
}

/*
 * @param outFile a filename to write the test outputs to (output will be appended)
 * @param outputFormat the output format of the tests (text,xml,...)
 *
 * @throw ComponentInitException
 */
TestRunner::TestRunner(std::string outputFile, TestRunnerOutputFormat outputFormat) :
   TestRunnerBase(outputFile, outputFormat)
{
}

TestRunner::~TestRunner()
{
}

bool TestRunner::addTests()
{
   Config* cfg = Program::getApp()->getConfig();
   if(cfg->getRunUnitTests())
   {
      this->testRunner.addTest(TestConfig::suite());
    //  this->testRunner.addTest(TestHelperdCommunication::suite());
      return true;
   }
   else
      return false;
}

