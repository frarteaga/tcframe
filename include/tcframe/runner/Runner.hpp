#pragma once

#include <iostream>

#include "Args.hpp"
#include "ArgsParser.hpp"
#include "RunnerLogger.hpp"
#include "RunnerLoggerFactory.hpp"
#include "tcframe/generator.hpp"
#include "tcframe/os.hpp"
#include "tcframe/spec.hpp"
#include "tcframe/testcase.hpp"
#include "tcframe/util.hpp"
#include "tcframe/verifier.hpp"

using std::cout;
using std::endl;

namespace tcframe {

template<typename TProblemSpec>
class Runner {
private:
    BaseTestSpec<TProblemSpec>* testSpec_;

    LoggerEngine* loggerEngine_;
    OperatingSystem* os_;

    RunnerLoggerFactory* loggerFactory_;
    GeneratorFactory* generatorFactory_;

public:
    Runner(BaseTestSpec<TProblemSpec>* testSpec)
            : testSpec_(testSpec)
            , loggerEngine_(new SimpleLoggerEngine())
            , os_(new UnixOperatingSystem())
            , loggerFactory_(new RunnerLoggerFactory())
            , generatorFactory_(new GeneratorFactory()) {}

    /* Visible for testing. */
    Runner(
            BaseTestSpec<TProblemSpec>* testSpec,
            LoggerEngine* loggerEngine,
            OperatingSystem* os,
            RunnerLoggerFactory* runnerLoggerFactory,
            GeneratorFactory* generatorFactory)
            : testSpec_(testSpec)
            , loggerEngine_(loggerEngine)
            , os_(os)
            , loggerFactory_(runnerLoggerFactory)
            , generatorFactory_(generatorFactory) {}

    int run(int argc, char* argv[]) {
        auto logger = loggerFactory_->create(loggerEngine_);

        try {
            Args args = parseArgs(argc, argv);
            CoreSpec coreSpec = buildCoreSpec(logger);
            return generate(args, coreSpec) ? 0 : 1;
        } catch (...) {
            return 1;
        }
    }

private:
    Args parseArgs(int argc, char* argv[]) {
        try {
            return ArgsParser::parse(argc, argv);
        } catch (runtime_error& e) {
            cout << e.what() << endl;
            throw;
        }
    }

    CoreSpec buildCoreSpec(RunnerLogger* logger) {
        try {
            return testSpec_->buildCoreSpec();
        } catch (runtime_error& e) {
            logger->logSpecificationFailure({e.what()});
            throw;
        }
    }

    bool generate(const Args& args, const CoreSpec& coreSpec) {
        const ProblemConfig& problemConfig = coreSpec.problemConfig();

        GeneratorConfig config = GeneratorConfigBuilder()
                .setMultipleTestCasesCount(problemConfig.multipleTestCasesCount().value_or(nullptr))
                .setSeed(args.seed().value_or(DefaultValues::seed()))
                .setSlug(args.slug().value_or(problemConfig.slug().value_or(DefaultValues::slug())))
                .setSolutionCommand(args.solution().value_or(DefaultValues::solutionCommand()))
                .setTestCasesDir(args.tcDir().value_or(DefaultValues::testCasesDir()))
                .build();

        auto ioManipulator = new IOManipulator(coreSpec.ioFormat());
        auto verifier = new Verifier(coreSpec.constraintSuite());
        auto logger = new GeneratorLogger(loggerEngine_);
        auto testCaseGenerator = new TestCaseGenerator(verifier, ioManipulator, os_, logger);
        auto generator = generatorFactory_->create(testCaseGenerator, verifier, os_, logger);

        auto testSuite = TestSuiteProvider::provide(
                coreSpec.rawTestSuite(),
                config.slug(),
                optional<IOManipulator*>(ioManipulator));

        return generator->generate(testSuite, config);
    }
};

}
