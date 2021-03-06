#include "gtest/gtest.h"

#include "tcframe/deprecated/iovariable/Matrix.hpp"
#include "tcframe/deprecated/iovariable/ParsingException.hpp"

#include <sstream>
#include <vector>

using std::istringstream;
using std::ostringstream;
using std::vector;

using tcframe_old::Matrix;
using tcframe_old::ParsingException;

TEST(DeprecatedMatrixTest, Construction) {
    vector<vector<int>> G;
    Matrix<int> wrappedG(G, "G");

    G = vector<vector<int>>{ {1, 2}, {3, 4, 5} };

    EXPECT_EQ("G", wrappedG.getName());
    ASSERT_EQ(2, wrappedG.rowsSize());
    EXPECT_EQ(2, wrappedG.columnsSize(0));
    EXPECT_EQ(3, wrappedG.columnsSize(1));
}

TEST(DeprecatedMatrixTest, Printing) {
    vector<vector<int>> G;
    Matrix<int> wrappedG(G, "G");

    G = vector<vector<int>>{ {1, 2}, {3, 4} };

    ostringstream sout;
    wrappedG.printElementTo(0, 0, sout);
    sout << " ";
    wrappedG.printElementTo(0, 1, sout);
    sout << "\n";
    wrappedG.printElementTo(1, 0, sout);
    sout << " ";
    wrappedG.printElementTo(1, 1, sout);
    sout << "\n";

    EXPECT_EQ("1 2\n3 4\n", sout.str());
}

TEST(DeprecatedMatrixTest, FailedParsingAndAdding) {
    vector<vector<int>> G;
    Matrix<int> wrappedG(G, "G");

    istringstream sin("1 2\na 4\n");

    try {
        wrappedG.addRow();
        wrappedG.parseAndAddColumnElementFrom(sin);
        sin.get();
        wrappedG.parseAndAddColumnElementFrom(sin);
        sin.get();
        wrappedG.addRow();
        wrappedG.parseAndAddColumnElementFrom(sin);
        FAIL();
    } catch (ParsingException& e) {
        EXPECT_EQ("Cannot parse for variable `G[1][0]`. Found: 'a'", e.getMessage());
    }

}

TEST(DeprecatedMatrixTest, ParsingAndAdding) {
    vector<vector<int>> G;
    Matrix<int> wrappedG(G, "G");

    istringstream sin("1 2\n3 4\n");

    wrappedG.addRow();
    wrappedG.parseAndAddColumnElementFrom(sin);
    sin.get();
    wrappedG.parseAndAddColumnElementFrom(sin);
    sin.get();
    wrappedG.addRow();
    wrappedG.parseAndAddColumnElementFrom(sin);
    sin.get();
    wrappedG.parseAndAddColumnElementFrom(sin);
    sin.get();

    EXPECT_EQ((vector<vector<int>>{ {1, 2}, {3, 4} }), G);
}

TEST(DeprecatedMatrixTest, Clearing) {
    vector<vector<int>> G;
    Matrix<int> wrappedG(G, "G");

    G = vector<vector<int>>{ {1, 2}, {3, 4} };
    wrappedG.clear();

    EXPECT_EQ(vector<vector<int>>(), G);
}
