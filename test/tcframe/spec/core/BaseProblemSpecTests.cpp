#include "gmock/gmock.h"

#include "tcframe/experimental/runner.hpp"

using ::testing::A;
using ::testing::AllOf;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Property;
using ::testing::SizeIs;
using ::testing::Test;

namespace tcframe {

class BaseProblemSpecTests : public Test {
protected:
    class ProblemSpec : public BaseProblemSpec {
    protected:
        int A, B;
        vector<int> X;
        vector<vector<int>> M;

        void Config() {
            setSlug("ab");
        }

        void InputFormat() {
            newLineIOSegment()
                    .addScalarVariable(Scalar::create(A, "A"))
                    .addScalarVariable(Scalar::create(B, "B"));
            newLinesIOSegment()
                    .addVectorVariable(Vector::create(X, "X"))
                    .setSize(3);
            newGridIOSegment()
                    .addMatrixVariable(Matrix::create(M, "M"))
                    .setSize(2, 3)
                    .build();
        }

        void OutputFormat() {
            newLinesIOSegment()
                    .addVectorVariable(Vector::create(X, "X"))
                    .setSize(3);
            newGridIOSegment()
                    .addMatrixVariable(Matrix::create(M, "M"))
                    .setSize(2, 3)
                    .build();
            newLineIOSegment()
                    .addScalarVariable(Scalar::create(A, "A"))
                    .addScalarVariable(Scalar::create(B, "B"));
        }
    };

    class ProblemSpecWithMultipleTestCasesConstraints : public BaseProblemSpec {
    protected:
        int T;

        void Config() {
            setSlug("ab");
            setMultipleTestCasesCount(T);
        }

        void InputFormat() {}

        void MultipleTestCasesConstraints() {
            addConstraint(Constraint([=] {return 1 <= T && T <= 20;}, "1 <= T && T <= 100"));
        }
    };

    class ProblemSpecWithConstraints : public ProblemSpec {
    protected:
        void Constraints() {
            addConstraint(Constraint([=] {return 1 <= A && A <= 100;}, "1 <= A && A <= 100"));
            addConstraint(Constraint([=] {return 1 <= B && B <= 100;}, "1 <= B && B <= 100"));
        }
    };

    class ProblemSpecWithSubtasks : public ProblemSpec {
    protected:
        void Subtask1() {
            addConstraint(Constraint([=] {return 1 <= A && A <= 100;}, "1 <= A && A <= 100"));
            addConstraint(Constraint([=] {return 1 <= B && B <= 100;}, "1 <= B && B <= 100"));
            addConstraint(Constraint([=] {return A != B;}, "A != B"));
        }

        void Subtask2() {
            addConstraint(Constraint([=] {return 1 <= A && A <= 1000;}, "1 <= A && A <= 1000"));
            addConstraint(Constraint([=] {return 1 <= B && B <= 1000;}, "1 <= B && B <= 1000"));
        }
    };
    
    ProblemSpec spec;
    ProblemSpecWithConstraints specWithConstraints;
    ProblemSpecWithMultipleTestCasesConstraints specWithMultipleTestCasesConstraints;
    ProblemSpecWithSubtasks specWithSubtasks;
};

TEST_F(BaseProblemSpecTests, Config) {
    ProblemConfig config = spec.buildProblemConfig();
    EXPECT_THAT(config.slug(), Eq(optional<string>("ab")));
}

TEST_F(BaseProblemSpecTests, IOFormat) {
    IOFormat ioFormat = spec.buildIOFormat();
    EXPECT_THAT(ioFormat.inputFormat(), ElementsAre(
        A<LineIOSegment*>(),
        A<LinesIOSegment*>(),
        A<GridIOSegment*>()));
    EXPECT_THAT(ioFormat.outputFormat(), ElementsAre(
            A<LinesIOSegment*>(),
            A<GridIOSegment*>(),
            A<LineIOSegment*>()));
    }

TEST_F(BaseProblemSpecTests, Constraints) {
    ConstraintSuite constraintSuite = specWithConstraints.buildConstraintSuite();
    EXPECT_THAT(constraintSuite.constraints(), ElementsAre(
        AllOf(Property(&Subtask::id, -1), Property(&Subtask::constraints, SizeIs(2)))));
}

TEST_F(BaseProblemSpecTests, MultipleTestCasesConstraints) {
    ConstraintSuite constraintSuite = specWithMultipleTestCasesConstraints.buildConstraintSuite();
    EXPECT_THAT(constraintSuite.multipleTestCasesConstraints(), SizeIs(1));
}

TEST_F(BaseProblemSpecTests, Subtasks) {
    ConstraintSuite constraintSuite = specWithSubtasks.buildConstraintSuite();
    EXPECT_THAT(constraintSuite.constraints(), ElementsAre(
            AllOf(Property(&Subtask::id, 1), Property(&Subtask::constraints, SizeIs(3))),
            AllOf(Property(&Subtask::id, 2), Property(&Subtask::constraints, SizeIs(2)))));
}

}
