#include <iostream>
#include <ostream>
#include <algorithm>
#include <map>
#include <memory>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <chrono>

typedef unsigned int u_int;
typedef unsigned char u_char;

namespace chess {

	const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

	enum PieceId { pawn, knight, bishop, rook, queen, king };
	const std::string PieceNames[] = { "pawn", "knight", "bishop", "rook", "queen", "king" };

	enum SquareId { empty, wpawn, bpawn, wknight, bknight, wbishop, bbishop, wrook, brook, 
		wqueen, bqueen, wking, bking };

	const char boardRows[] = { '8', '7', '6', '5', '4', '3', '2', '1' };
	const char boardColumns[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

	bool getPieceColor(SquareId piece) {  // true = white, false = black
		if (piece == empty) throw "empty peice has no color";
		return piece % 2;
	}

	namespace pieceMovement {
		// not necessarly legal movement

		std::vector<int8_t> computePossibleMoves_WhitePawn_NoEnPassant(const std::array<SquareId, 64>& board, int8_t pos) {  // ordered
			std::vector<int8_t> res;
			if (pos >= 48 && pos <= 55 && board[pos - 8] == empty && board[pos - 16] == empty) res.push_back(pos - 16);
			if (pos % 8 != 0 && board[pos - 9] != empty && !getPieceColor(board[pos - 9])) res.push_back(pos - 9);
			if (board[pos - 8] == empty) res.push_back(pos - 8);
			if (pos % 8 != 7 && board[pos - 7] != empty && !getPieceColor(board[pos - 7])) res.push_back(pos - 7);
			return res;
		}

		std::vector<int8_t> computePossibleMoves_BlackPawn_NoEnPassant(const std::array<SquareId, 64>& board, int8_t pos) {  // ordered
			std::vector<int8_t> res;
			if (pos % 8 != 0 && board[pos + 7] != empty && getPieceColor(board[pos + 7])) res.push_back(pos + 7);
			if (board[pos + 8] == empty) res.push_back(pos + 8);
			if (pos % 8 != 7 && board[pos + 9] != empty && getPieceColor(board[pos + 9])) res.push_back(pos + 9);
			if (pos >= 8 && pos <= 15 && board[pos + 8] == empty && board[pos + 16] == empty) res.push_back(pos + 16);
			return res;
		}

		std::vector<int8_t> computePossibleMoves_WhitePawn_WithEnPassant(const std::array<SquareId, 64>& board, int8_t pos, int8_t enPassantTarget) {  // ordered
			std::vector<int8_t> res;
			if (pos >= 48 && pos <= 55 && board[pos - 8] == empty && board[pos - 16] == empty) res.push_back(pos - 16);
			if (enPassantTarget == pos - 9 || (pos % 8 != 0 && board[pos - 9] != empty && !getPieceColor(board[pos - 9]))) res.push_back(pos - 9);
			if (board[pos - 8] == empty) res.push_back(pos - 8);
			if (enPassantTarget == pos - 7 || (pos % 8 != 7 && board[pos - 7] != empty && !getPieceColor(board[pos - 7]))) res.push_back(pos - 7);
			return res;
		}

		std::vector<int8_t> computePossibleMoves_BlackPawn_WithEnPassant(const std::array<SquareId, 64>& board, int8_t pos, int8_t enPassantTarget) {  // ordered
			std::vector<int8_t> res;
			if (enPassantTarget == pos + 7 || (pos % 8 != 0 && board[pos + 7] != empty && getPieceColor(board[pos + 7]))) res.push_back(pos + 7);
			if (board[pos + 8] == empty) res.push_back(pos + 8);
			if (enPassantTarget == pos + 9 || (pos % 8 != 7 && board[pos + 9] != empty && getPieceColor(board[pos + 9]))) res.push_back(pos + 9);
			if (pos >= 8 && pos <= 15 && board[pos + 8] == empty && board[pos + 16] == empty) res.push_back(pos + 16);
			return res;
		}

		std::vector<int8_t> computePossibleMoves_Knight(const std::array<SquareId, 64>& board, int8_t pos, bool color) {  // ordered
			div_t d = std::div(pos, 8);
			int8_t x = d.rem;
			int8_t y = d.quot;
			std::vector<int8_t> res;
			if (x > 0 && y > 1 && (board[pos - 17] == empty || color != getPieceColor(board[pos - 17]))) res.push_back(pos - 17);
			if (x < 7 && y > 1 && (board[pos - 15] == empty || color != getPieceColor(board[pos - 15]))) res.push_back(pos - 15);
			if (x > 1 && y > 0 && (board[pos - 10] == empty || color != getPieceColor(board[pos - 10]))) res.push_back(pos - 10);
			if (x < 6 && y > 0 && (board[pos -  6] == empty || color != getPieceColor(board[pos -  6]))) res.push_back(pos -  6);
			if (x > 1 && y < 7 && (board[pos +  6] == empty || color != getPieceColor(board[pos +  6]))) res.push_back(pos +  6);
			if (x < 6 && y < 7 && (board[pos + 10] == empty || color != getPieceColor(board[pos + 10]))) res.push_back(pos + 10);
			if (x > 0 && y < 6 && (board[pos + 15] == empty || color != getPieceColor(board[pos + 15]))) res.push_back(pos + 15);
			if (x < 7 && y < 6 && (board[pos + 17] == empty || color != getPieceColor(board[pos + 17]))) res.push_back(pos + 17);
			return res;
		}

		std::vector<int8_t> computePossibleMoves_Bishop(const std::array<SquareId, 64>& board, int8_t pos, bool color) {  // NOT ordered
			div_t d = std::div(pos, 8);
			int8_t x = d.rem;
			int8_t y = d.quot;
			std::vector<int8_t> res;
			for (int8_t i = pos - 7; i >= pos - 7 * std::min<int8_t>(7 - x, y); i -= 7) {  // up-right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 9; i <= pos + 9 * std::min<int8_t>(7 - x, 7 - y); i += 9) {  // down-right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 7; i <= pos + 7 * std::min<int8_t>(x, 7 - y); i += 7) {  // down-left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos - 9; i >= pos - 9 * std::min<int8_t>(x, y); i -= 9) {  // up-left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			return res;
		}

		std::vector<int8_t> computePossibleMoves_Rook(const std::array<SquareId, 64>& board, int8_t pos, bool color) {  // ordered
			int8_t y = pos / 8;
			std::vector<int8_t> res;
			for (int8_t i = pos - 1; i >= 8 * y; i--) {  // left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos - 8; i >= 0; i -= 8) {  // up
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			std::reverse(res.begin(), res.end());
			for (int8_t i = pos + 1; i < (y + 1) * 8; i++) {  // right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 8; i < 64; i += 8) {  // down
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			return res;
		}

		std::vector<int8_t> computePossibleMoves_Queen(const std::array<SquareId, 64>& board, int8_t pos, bool color) {  // unordered
			div_t d = std::div(pos, 8);
			int8_t x = d.rem;
			int8_t y = d.quot;
			std::vector<int8_t> res;
			for (int8_t i = pos - 7; i >= pos - 7 * std::min<int8_t>(7 - x, y); i -= 7) {  // up-right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 9; i <= pos + 9 * std::min<int8_t>(7 - x, 7 - y); i += 9) {  // down-right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 7; i <= pos + 7 * std::min<int8_t>(x, 7 - y); i += 7) {  // down-left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos - 9; i >= pos - 9 * std::min<int8_t>(x, y); i -= 9) {  // up-left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos - 1; i >= 8 * y; i--) {  // left
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos - 8; i >= 0; i -= 8) {  // up
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			std::reverse(res.begin(), res.end());
			for (int8_t i = pos + 1; i < (y + 1) * 8; i++) {  // right
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			for (int8_t i = pos + 8; i < 64; i += 8) {  // down
				if (board[i] != empty && color == getPieceColor(board[i])) break;
				res.push_back(i);
				if (board[i] != empty && color != getPieceColor(board[i])) break;
			}
			return res;
		}


		// NOT including castling
		std::vector<int8_t> computePossibleMoves_King_Simple(const std::array<SquareId, 64>& board, int8_t pos, bool color) {  // ordered
			div_t d = std::div(pos, 8);
			int8_t x = d.rem;
			int8_t y = d.quot;
			std::vector<int8_t> res;
			bool canGoUp = (y != 0);
			bool canGoDown = (y != 7);
			bool canGoRight = (x != 7);
			bool canGoLeft = (x != 0);
			if (canGoLeft 	&& canGoUp 		&& (board[pos - 9] == empty || color != getPieceColor(board[pos - 9])))	res.push_back(pos - 9);
			if (canGoUp 						&& (board[pos - 8] == empty || color != getPieceColor(board[pos - 8])))	res.push_back(pos - 8);
			if (canGoUp 	&& canGoRight 	&& (board[pos - 7] == empty || color != getPieceColor(board[pos - 7])))	res.push_back(pos - 7);
			if (canGoLeft 						&& (board[pos - 1] == empty || color != getPieceColor(board[pos - 1])))	res.push_back(pos - 1);
			if (canGoRight 					&& (board[pos + 1] == empty || color != getPieceColor(board[pos + 1])))	res.push_back(pos + 1);
			if (canGoDown 	&& canGoLeft 	&& (board[pos + 7] == empty || color != getPieceColor(board[pos + 7])))  	res.push_back(pos + 7);
			if (canGoDown 						&& (board[pos + 8] == empty || color != getPieceColor(board[pos + 8])))	res.push_back(pos + 8);
			if (canGoRight && canGoDown 	&& (board[pos + 9] == empty || color != getPieceColor(board[pos + 9])))	res.push_back(pos + 9);
			return res;
		}

		std::vector<int8_t> computePossiblePieceMoves_Simple(const std::array<SquareId, 64>& board, int8_t pos, int8_t enPassantTarget, bool color) { //TODO
			if (pos > 63) throw "position number too big 235231";
			if (board[pos] == empty) throw "empty square has no movement";
			if (enPassantTarget < -1 || enPassantTarget > 63) throw "wrong enpassant 460186";
			if (board[pos] == wpawn && enPassantTarget == -1) return computePossibleMoves_WhitePawn_NoEnPassant(board, pos);
			if (board[pos] == wpawn && enPassantTarget != -1) return computePossibleMoves_WhitePawn_WithEnPassant(board, pos, enPassantTarget);
			if (board[pos] == bpawn && enPassantTarget == -1) return computePossibleMoves_BlackPawn_NoEnPassant(board, pos);
			if (board[pos] == bpawn && enPassantTarget != -1) return computePossibleMoves_BlackPawn_WithEnPassant(board, pos, enPassantTarget);
			if (board[pos] == wknight || board[pos] == bknight) return computePossibleMoves_Knight(board, pos, color);
			if (board[pos] == wbishop || board[pos] == bbishop) return computePossibleMoves_Bishop(board, pos, color);
			if (board[pos] == wrook || board[pos] == brook) return computePossibleMoves_Rook(board, pos, color);
			if (board[pos] == wqueen || board[pos] == bqueen) return computePossibleMoves_Queen(board, pos, color);
			if (board[pos] == wking || board[pos] == bking) return computePossibleMoves_King_Simple(board, pos, color);
			throw "random error 134125";
		}


		int8_t findKing(const std::array<SquareId, 64>& board, bool color) {
			SquareId searchFor = color ? wking : bking;
			for (int8_t i = 0; i < 64; i++) {
				if (board[i] == searchFor) {
					return i;
				}
			}
			throw "findKing error 23i5u2395";
		}

		bool isSquareAttacked(const std::array<SquareId, 64>& board, bool color, int8_t pos) {
			/*
			- compute rook movement from king : if movement square is queen or rook ret true
			- compute bishop movemet from king : if movement square is queen or bishop ret true
			- compute knight movement from king: if movement square is knight ret true
			- compute king movement from king: if movement square is king ret true
			- compute pawn squares and check if pawn
			*/

			if (color) {

				if (pos > 8) {
					if (pos % 8 != 0 && board[pos - 9] == bpawn) return true;
					if (pos % 8 != 7 && board[pos - 7] == bpawn) return true;
				}
				for (int8_t p : computePossibleMoves_Rook(board, pos, color)) {
					if (board[p] == brook || board[p] == bqueen) return true;
				}
				for (int8_t p : computePossibleMoves_Bishop(board, pos, color)) {
					if (board[p] == bbishop || board[p] == bqueen) return true;
				}
				for (int8_t p : computePossibleMoves_Knight(board, pos, color)) {
					if (board[p] == bknight) return true;
				}
				for (int8_t p : computePossibleMoves_King_Simple(board, pos, color)) {
					if (board[p] == bking) return true;
				}
				

			} else {

				if (pos < 56) {
					if (pos % 8 != 0 && board[pos + 7] == wpawn) return true;
					if (pos % 8 != 7 && board[pos + 9] == wpawn) return true;
				}
				for (int8_t p : computePossibleMoves_Rook(board, pos, color)) {
					if (board[p] == wrook || board[p] == wqueen) return true;
				}
				for (int8_t p : computePossibleMoves_Bishop(board, pos, color)) {
					if (board[p] == wbishop || board[p] == wqueen) return true;
				}
				for (int8_t p : computePossibleMoves_Knight(board, pos, color)) {
					if (board[p] == wknight) return true;
				}
				for (int8_t p : computePossibleMoves_King_Simple(board, pos, color)) {
					if (board[p] == wking) return true;
				}
				
			}

			return false;
		}


		bool isOnCornerOfBoard(int8_t pos) {
			return (pos == 0 || pos == 7 || pos == 56 || pos == 63);
		}

		bool isOnEdgeOfBoard(int8_t pos) {
			return ((pos >= 0 && pos <= 8) || (pos >= 55 && pos <= 63) || (pos % 8 == 0) || (pos % 8 == 7));
		}

		bool isInSameLineOrDiagonal(int8_t pos1, int8_t pos2) {
			div_t d1 = std::div(pos1, 8);
			int8_t x1 = d1.rem;
			int8_t y1 = d1.quot;
			div_t d2 = std::div(pos2, 8);
			int8_t x2 = d2.rem;
			int8_t y2 = d2.quot;
			return (x1 == x2 || y1 == y2 || std::abs(x2 - x1) == std::abs(y2 - y1));
		}



		std::vector<std::pair<int8_t, int8_t>> computeLegalMoves_Simple(const std::array<SquareId, 64>& board, bool color, int8_t enPassantTarget) {
			std::vector<std::pair<int8_t, int8_t>> res;
			int8_t kingSquare = findKing(board, color);
			for (int8_t i = 0; i < 64; i++) {
				if (board[i] != empty && getPieceColor(board[i]) == color) {  // if pieces of right color...

					if (board[i] == wking || board[i] == bking) {
						for (int8_t j : computePossibleMoves_King_Simple(board, i, color)) {
							if (!isSquareAttacked(board, color, j)) {
								res.push_back(std::pair<int8_t, int8_t>(i, j));
							}
						}

					} else {

						// if i could allow discoveries: check for legality
						if (board[i] == wpawn || board[i] == bpawn || (!isOnEdgeOfBoard(i) && isInSameLineOrDiagonal(i, kingSquare))) {
							for (int8_t j : computePossiblePieceMoves_Simple(board, i, enPassantTarget, color)) {
								std::array<SquareId, 64> boardCopy = board;
								boardCopy[j] = boardCopy[i];
								boardCopy[i] = empty;
								if (!isSquareAttacked(boardCopy, color, kingSquare)) {
									res.push_back(std::pair<int8_t, int8_t>(i, j));
								}
							}
						} else {
							for (int8_t j : computePossiblePieceMoves_Simple(board, i, enPassantTarget, color)) {
								res.push_back(std::pair<int8_t, int8_t>(i, j));
							}
						}
					}
				}
			}
			return res;
		}



		// // includes castling
		// std::vector<std::pair<int8_t, int8_t>> computeLegalMoves_Full(const std::array<SquareId, 64>& board, const bool color, const int8_t enPassantTarget, const std::array<bool, 4>& castlingAvailability) {
		// 	std::vector<std::pair<int8_t, int8_t>> res = computeLegalMoves_Simple(board, color, enPassantTarget);
		// 	std::vector<std::pair<int8_t, int8_t>> res2 = computeCastlingMoves(board, color, castlingAvailability);
		// 	res.insert(res.end(), res2.begin(), res2.end());
		// 	return res;
		// }

		// std::vector<std::pair<int8_t, int8_t>> computeCastlingMoves(const std::array<SquareId, 64>& board, const bool color, const std::array<bool, 4>& castlingAvailability) {  //TODO
		// 	std::vector<std::pair<int8_t, int8_t>> res;
		// 	if (castlingAvailability[0] || castlingAvailability[1] || castlingAvailability[2] || castlingAvailability[3]) {
		// 		return res;
		// 	}
		// 	if (!isKingAttacked(board, color)) {
		// 		if (color) {  //white
		// 			if (board[60] == wking) {
		// 				if (board[61] == empty && board[62] == empty) {  // kingside castle
		// 					std::array<SquareId, 64> boardCopy = board;
		// 					boardCopy[61] = boardCopy[60];
		// 					boardCopy[60] = empty;

		// 				}
		// 				if () {  // queenside castle

		// 				}
		// 			}
		// 		} else {  // black
		// 			if (board[4] == bking) {
		// 				if () {  // kingside castle

		// 				}
		// 				if () {  // queenside castle

		// 				}
		// 			}
		// 		}
		// 	}
		// 	return res;
		// }

	}

	namespace convert {

		const char squareIdChars[] = { '1', 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k' };
		const char fenAllowedCharsPart1[] = { '/', '1', '2', '3', '4', '5', '6', '7', '8', 'P', 'p', 'N', 'n', 'B', 'b', 'R', 'r', 'Q', 'q', 'K', 'k' };

		

		

		// bool isPositionLegal() {return false;} //TODO

		std::string computeBoardToString(std::array<SquareId, 64> arr) {
			std::string s;
			for (int8_t i = 0; i < 64; i++) {
				s += chess::convert::squareIdChars[arr[i]];
				if ((i + 1) % 8) {
					s += ' ';
				} else {
					s += '\n';
				}
			}
			return s;
		}

		std::array<SquareId, 64> computeBoardFromFenPart(std::string fen) {
			if (fen.length() > 71) {
				throw "fen too long";
			}
			if (fen.length() < 15) {
				throw "fen too short";
			}
			int8_t slashCount = 0;
			std::vector<std::string> vec(8);
			for (char c : fen) {
				if (std::find(std::begin(fenAllowedCharsPart1), std::end(fenAllowedCharsPart1), c) == std::end(fenAllowedCharsPart1)) {
					throw "fen part not valid";
				}
				if (c == '/') {
					if (slashCount == 7) {
						throw "invalid slashcount";
					}
					slashCount++;
				} else {
					vec[slashCount] += c;
				}
			}
			if (slashCount != 7) {
				throw "invalid slashcount";
			}
			std::string fenRedone;
			for (std::string str : vec) {
				int8_t count = 0;
				for (char c : str) {
					if (49 <= c && c <= 57) {  // is number 1-8
						count += c - 48;
						fenRedone += std::string(c - 48, '1');
					} else {
						count += 1;
						fenRedone += c;
					}
				}
				if (count != 8) {
					throw "bad count in fen";
				}
			}
			std::array<SquareId, 64> ret;
			for (int8_t i = 0; i < 64; i++) {
				ret[i] = chess::SquareId(std::find(squareIdChars, std::end(squareIdChars), fenRedone[i]) - squareIdChars);
			}
			return ret;
		}

		std::string computeFenPartFromBoard(std::array<SquareId, 64> board) {
			std::string str, ret;
			for (int8_t i = 0; i < 64; i++) {
				if (i % 8 == 0) {
					str += '/';
				}
				str += squareIdChars[board[i]];
			}
			int8_t count = 0;
			for (int8_t i = 1; i < 72; i++) {
				if (str[i] == '1') {
					count++;
				} else {
					if (count != 0) {
						ret += count + 48;
						count = 0;
					}
					ret += str[i];
				}
			}
			if (count) {
				ret += count + 48;
			}
			return ret;
		}

		std::string getCoordsFromIndex(int8_t index) {
			if (index > 63 || index < 0) {
				throw "index must be between 0 and 63";
			}
			div_t d = std::div(index, 8);
			return std::string{boardColumns[d.rem], boardRows[d.quot]};
		}

		int8_t getIndexFromCoords(std::string str) {
			if (str.length() != 2) {
				throw "string need to be 2";
			}
			const auto column = std::find(boardColumns, boardColumns + 8, str[0]) - boardColumns;
			const auto row = std::find(boardRows, boardRows + 8, str[1]) - boardRows;
			if (row == 8 || column == 8) {
				throw "chars not vaild";
			}
			return 8 * row + column;
		}
	}


	namespace symmetry {
		enum SymmetryId	{ noTurn_noSymmetry, noTurn_verticalSymmetry, noTurn_horizontalSymmetry, noTurn_doubleSymmetry,
			withTurn_noSymmetry, withTurn_verticalSymmetry, withTurn_horizontalSymmetry, withTurn_doubleSymmetry };

		std::array<SquareId, 64> turnClockwise(const std::array<SquareId, 64>& board) {
			std::array<SquareId, 64> retBoard = {};
			for (int8_t pos = 0; pos < 64; pos++) {
				if (board[pos] != empty) {
					div_t d = std::div(pos, 8);
					int8_t x = d.rem;
					int8_t y = d.quot;
					int8_t res_x = 7 - y;
					int8_t res_y = x;
					retBoard[res_y * 8 + res_x] = board[pos];
				}
			}
			return retBoard;
		}

		std::array<SquareId, 64> mirrorVertically(const std::array<SquareId, 64>& board) {
			std::array<SquareId, 64> retBoard = {};
			for (int8_t pos = 0; pos < 64; pos++) {
				if (board[pos] != empty) {
					div_t d = std::div(pos, 8);
					int8_t x = d.rem;
					int8_t y = d.quot;
					int8_t res_x = 7 - x;
					int8_t res_y = y;
					retBoard[res_y * 8 + res_x] = board[pos];
				}
			}
			return retBoard;
		}

		std::array<SquareId, 64> mirrorHorizontally(const std::array<SquareId, 64>& board) {
			std::array<SquareId, 64> retBoard = {};
			for (int8_t pos = 0; pos < 64; pos++) {
				if (board[pos] != empty) {
					div_t d = std::div(pos, 8);
					int8_t x = d.rem;
					int8_t y = d.quot;
					int8_t res_x = x;
					int8_t res_y = 7 - y;
					retBoard[res_y * 8 + res_x] = board[pos];
				}
			}
			return retBoard;
		}

		std::array<SquareId, 64> mirrorBothWays(const std::array<SquareId, 64>& board) {
			std::array<SquareId, 64> retBoard = {};
			for (int8_t pos = 0; pos < 64; pos++) {
				if (board[pos] != empty) {
					div_t d = std::div(pos, 8);
					int8_t x = d.rem;
					int8_t y = d.quot;
					int8_t res_x = 7 - x;
					int8_t res_y = 7 - y;
					retBoard[res_y * 8 + res_x] = board[pos];
				}
			}
			return retBoard;
		}


		// enum SymmetryId	{ noTurn_noSymmetry, noTurn_verticalSymmetry, noTurn_horizontalSymmetry, noTurn_doubleSymmetry,
		// 	withTurn_noSymmetry, withTurn_verticalSymmetry, withTurn_horizontalSymmetry, withTurn_doubleSymmetry };
		std::array<SquareId, 64> computeSymmetry(SymmetryId symId, const std::array<SquareId, 64>& board) {
			switch(symId) {
			case noTurn_noSymmetry:
				return std::array<SquareId, 64>(board);
			case noTurn_verticalSymmetry:
				return mirrorVertically(board);
			case noTurn_horizontalSymmetry:
				return mirrorHorizontally(board);
			case noTurn_doubleSymmetry:
				return mirrorBothWays(board);
			default:
				return computeSymmetry(SymmetryId(int(symId) - 4), turnClockwise(board));
			}
		}

	}

	class Position {
		// a8 = 0, b8 = 1, ..., a7 = 8, b7 = 9, ..., h1 = 63
		// assume position object is well initialized (aka valid position)
		private:
			
		public:
			const std::array<SquareId, 64> board;
			const bool activeColor;  // true = white, false = black
			const std::array<bool, 4> castlingAvailability; 
			// order: white kingside, white queenside, black kingside, black gueenside
			const int8_t enPassantTarget;  // -1 if does not exist, 0-63 if it does
			const int8_t halfmoveClock; // 0->50
			const int16_t fullmoveNumber; // 1->inf

			Position(const std::array<SquareId, 64>& board, bool activeColor) : 
			board(board), activeColor(activeColor), castlingAvailability({false, false, false, false}), enPassantTarget(-1), 
			halfmoveClock(0), fullmoveNumber(1) {

			}

			Position(const std::array<SquareId, 64>& board, bool activeColor, const std::array<bool, 4>& castlingAvailability, 
				int8_t enPassantTarget, int8_t halfmoveClock, int16_t fullmoveNumber) : 
			board(board), activeColor(activeColor), castlingAvailability(castlingAvailability), enPassantTarget(enPassantTarget), 
			halfmoveClock(halfmoveClock), fullmoveNumber(fullmoveNumber) {
				//TODO

				// is en passant logical? (check if enemy pawn if after the square, 
				// 	check if ally pawns are close to square, check if en passant square is empty)

				// check if castling is logical (check if king on native square - e1 or e8 -, 
				// 	if  ally rooks are on native squares, and if king pathway is out of check)
			}

			~Position() {}

			const std::array<chess::SquareId, 64> getBoard() {
				return board;
			}

			std::string computeFen() {
				return convert::computeFenPartFromBoard(board);
			}

			std::string computeBoardAsString() {
				return convert::computeBoardToString(board);
			}

			Position copy() {
				return Position(board, activeColor, castlingAvailability, enPassantTarget, halfmoveClock, fullmoveNumber);
			}
			// std::vector<int8_t> computeMovementSquaresPART(int8_t pos) { //TODO
			// 	// return convert::computeMovementSquares(board, pos, enPassantTarget, castlingAvailability);
			// }
	};

}


int run(int argc, char const *argv[]) {
	
	// kingFen = 8/P1Kp4/k2p4/K2P1kp1/p3kp2/3P4/2Kk1Kp1/8;
	// chess::Position rookTest = chess::Position(chess::convert::computeBoardFromFenPart("P4r1p/2R5/1P1P2R1/r1p5/1r2r1p1/5R2/1R6/r1Pp2PR"), true, std::array<bool, 4>(), -1, 0, 1);
	// chess::Position bishopTest = chess::Position(chess::convert::computeBoardFromFenPart("4B2b/b1b5/5B2/1B1b2b1/5B2/2b5/1B1Bb3/6b1"), true, std::array<bool, 4>(), -1, 0, 1);
	// chess::Position queenTest = chess::Position(chess::convert::computeBoardFromFenPart("q3q3/8/6Q1/3Q4/4q3/1q6/Q3Q3/6Q1"), true, std::array<bool, 4>(), -1, 0, 1);
	// chess::Position knightTest = chess::Position(chess::convert::computeBoardFromFenPart("n4n1N/N1N1N3/N3n1n1/2N2N2/nN1nN2n/n7/2n1Nn2/N6N"), true, std::array<bool, 4>(), -1, 0, 1);
	// chess::Position pawnTest = chess::Position(chess::convert::computeBoardFromFenPart("2k5/1p5p/P6p/4PpP1/3P4/2P5/2PP4/4K3"), true, std::array<bool, 4>(), chess::convert::getIndexFromCoords("f6"), 0, 1);
	// auto movement = bishopTest.computeMovementSquaresPART(chess::convert::getIndexFromCoords("g1"));
	// for (int i = 0; i < 1000000; i++) p1.computeMovementSquaresPART(chess::convert::getIndexFromCoords("c2"));
	// for (auto& i : movement) {
		// std::cout << chess::convert::getCoordsFromIndex(i) << " ";
		// std::cout << (int) i << " ";
	// }
	// std::cout << std::endl;
	
	// std::string fen = "rnb1kbnr/ppp1pppp/8/1B1q4/8/8/PPPP1PPP/RNBQK1NR";
	// std::vector<std::pair<int8_t, int8_t>> legalMoves = chess::pieceMovement::computeLegalMoves_Simple(chess::convert::computeBoardFromFenPart(fen), false, -1);
	// std::cout << "Count: " << legalMoves.size() << std::endl;
	// for (auto& p : legalMoves) {
	// 	std::cout << "(" << chess::convert::getCoordsFromIndex(p.first) << ", " << chess::convert::getCoordsFromIndex(p.second) << ")  ";
	// }
	// std::cout << std::endl;

	// std::cout << chess::convert::computeFenPartFromBoard(chess::symmetry::turnClockwise(chess::convert::computeBoardFromFenPart(fen))) << std::endl;
	// std::cout << chess::convert::computeFenPartFromBoard(chess::symmetry::mirrorVertically(chess::convert::computeBoardFromFenPart(fen))) << std::endl;
	// std::cout << chess::convert::computeFenPartFromBoard(chess::symmetry::mirrorHorizontally(chess::convert::computeBoardFromFenPart(fen))) << std::endl;
	// std::cout << chess::convert::computeFenPartFromBoard(chess::symmetry::mirrorBothWays(chess::convert::computeBoardFromFenPart(fen))) << std::endl;
	// for (int i = 0; i < 8; i++) {
	// 	std::cout << chess::convert::computeFenPartFromBoard(chess::symmetry::computeSymmetry(chess::symmetry::SymmetryId(i), 
	// 		chess::convert::computeBoardFromFenPart("8/8/8/4rk2/3Q4/3K4/8/8"))) << std::endl;
	// }


	std::shared_ptr<chess::Position> pos = std::shared_ptr<chess::Position>(
		new chess::Position(chess::convert::computeBoardFromFenPart("r1bqkbnr/ppp1pppp/2n5/1B1p4/4P3/P7/1PPP1PPP/RNBQK1NR"), 0));
	auto legalMoves = chess::pieceMovement::computeLegalMoves_Simple(pos->board, pos->activeColor, pos->enPassantTarget);
	std::cout << legalMoves.size() << std::endl;
	for (auto& p : legalMoves) {
		std::cout << "(" << chess::convert::getCoordsFromIndex(p.first) << ", " << chess::convert::getCoordsFromIndex(p.second) << ")  ";
	}
	// std::cout << chess::pieceMovement::isInSameLineOrDiagonal(56, 9) << std::endl;

	// std::cout << << std::endl;
	return 0;
}

int main(int argc, char const *argv[]) {
	try {
		auto begin = std::chrono::high_resolution_clock::now();
		run(argc, argv);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns" << std::endl;
	} catch (const char* s) {
		std::cerr << "ERROR: " << s << std::endl;
	}
	return 0;
}

