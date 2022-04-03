#include <C:\Users\Teodor\Desktop\CppProjects\chess\chess.cpp>

using namespace chess;

namespace benchmark {

}

void BenckmarkAction(const std::array<chess::SquareId, 64>& board) {
	chess::pieceMovement::computeLegalMoves_Simple(board, true, -1);
	chess::pieceMovement::computeLegalMoves_Simple(board, false, -1);	
}

int main(int argc, char const *argv[])
{
	
	// CHANGE THIS
	// std::array<chess::SquareId, 64> testBoard = chess::convert::computeBoardFromFenPart("rnb1kbnr/ppp1pppp/8/1B1q4/8/8/PPPP1PPP/RNBQK1NR");
	// std::array<chess::SquareId, 64> testBoard = chess::convert::computeBoardFromFenPart("rnbqkbnr/ppp1pppp/8/3P4/B7/5P2/PPPP2PP/RNBQK1NR");
	std::array<chess::SquareId, 64> testBoard = chess::convert::computeBoardFromFenPart("8/8/3pk3/3P4/2K5/8/8/8");
	// std::array<chess::SquareId, 64> testBoard = chess::convert::computeBoardFromFenPart(chess::DEFAULT_FEN);
	u_int TEST_TIMES = 33333;
	#define FUNCTION BenckmarkAction(testBoard)
	// CHANGE THIS


	
	auto begin = std::chrono::high_resolution_clock::now();
	for (u_int i = 1; i < TEST_TIMES; i++) {
		FUNCTION;
		if (i % (TEST_TIMES / 100) == 0) {
			auto timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-begin).count();
			std::cout << i / (TEST_TIMES / 100) << "% | Avg time: " << timeDiff / i 
				<< "ns | Time Remaining: " << ((100 - (i / (TEST_TIMES / 100))) * (100 * timeDiff / (i / (TEST_TIMES / 100)))) / 100000000000 << "s" << std::endl;
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count();
	std::cout << diff / TEST_TIMES << "ns/step | Total: " << diff / 1000000 << "ms" << std::endl;
	return 0;
}

// computeLegalMoves_Simple : 161810ns