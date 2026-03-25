#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <climits>

using namespace std;

// Enums for better type safety
enum Color {
    WHITE, BLACK
};

enum PieceType {
    KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

enum GameStatus {
    WAITING, IN_PROGRESS, COMPLETED, ABORTED
};

// Forward declarations
class Board;
class Piece;
class Match;
class User;

// Position class to represent coordinates
class Position {
private:
    int row;
    int col;

public:
    Position() {
        row = 0;
        col = 0;
    }
    
    Position(int r, int c) {
        row = r;
        col = c;
    }
    
    int getRow() const { 
        return row; 
    }
    int getCol() const { 
        return col; 
    }
    
    bool isValid() const {
        return row >= 0 && row < 8 && col >= 0 && col < 8;
    }
    
    bool operator==(const Position& other) const {
        return row == other.row && col == other.col;
    }
    
    bool operator<(const Position& other) const {
        if (row != other.row) return row < other.row;
        return col < other.col;
    }
    
    string toString() const {
        return "(" + to_string(row) + "," + to_string(col) + ")";
    }
    
    // Convert to chess notation (e.g., e4, f7)
    string toChessNotation() const {
        char file = 'a' + col;
        char rank = '8' - row;
        return string(1, file) + string(1, rank);
    }
};

// Move class to represent a chess move
class Move {
private:
    Position from;
    Position to;
    Piece* piece;
    Piece* capturedPiece;

public:
    Move() {
        piece = nullptr;
        capturedPiece = nullptr;
    }
    
    Move(Position f, Position t, Piece* p, Piece* captured) {
        from = f;
        to = t;
        piece = p;
        capturedPiece = captured;
    }
    
    Position getFrom() const { 
        return from; 
    }
    Position getTo() const { 
        return to; 
    }
    Piece* getPiece() const { 
        return piece; 
    }
    Piece* getCapturedPiece() const { 
        return capturedPiece; 
    }
};

// Abstract Piece class following Strategy Pattern
class Piece {
protected:
    Color color;
    PieceType type;
    bool hasMoved;

public:
    Piece(Color c, PieceType t) {
        color = c;
        type = t;
        hasMoved = false;
    }
    
    virtual ~Piece() {}
    
    Color getColor() const { 
        return color; 
    }
    PieceType getType() const { 
        return type; 
    }
    bool getHasMoved() const { 
        return hasMoved; 
    }
    void setMoved(bool moved) { 
        hasMoved = moved; 
    }
    
    virtual vector<Position> getPossibleMoves(Position currentPos, Board* board) = 0;
    virtual string getSymbol() = 0;
    
    string toString() {
        string colorStr = (color == WHITE) ? "W" : "B";
        return colorStr + getSymbol();
    }
};

// Concrete Piece implementations
class King : public Piece {
public:
    King(Color color) : Piece(color, KING) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "K"; 
    }
};

class Queen : public Piece {
public:
    Queen(Color color) : Piece(color, QUEEN) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "Q"; 
    }
};

class Rook : public Piece {
public:
    Rook(Color color) : Piece(color, ROOK) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "R"; 
    }
};

class Bishop : public Piece {
public:
    Bishop(Color color) : Piece(color, BISHOP) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "B"; 
    }
};

class Knight : public Piece {
public:
    Knight(Color color) : Piece(color, KNIGHT) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "N"; 
    }
};

class Pawn : public Piece {
public:
    Pawn(Color color) : Piece(color, PAWN) {}
    
    vector<Position> getPossibleMoves(Position currentPos, Board* board) override;
    string getSymbol() override { 
        return "P"; 
    }
};

// Factory Pattern for creating pieces
class PieceFactory {
public:
    static Piece* createPiece(PieceType type, Color color) {
        switch (type) {
            case KING: return new King(color);
            case QUEEN: return new Queen(color);
            case ROOK: return new Rook(color);
            case BISHOP: return new Bishop(color);
            case KNIGHT: return new Knight(color);
            case PAWN: return new Pawn(color);
            default: return nullptr;
        }
    }
};

// Board class - Dumb object that manages pieces
class Board {
private:
    Piece* board[8][8];
    map<Position, Piece*> piecePositions;

public:
    Board() {
        // Initialize board to null
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                board[i][j] = nullptr;
            }
        }
        initializeBoard();
    }
    
    ~Board() {
        // Clean up pieces safely
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (board[i][j] != nullptr) {
                    delete board[i][j];
                    board[i][j] = nullptr;
                }
            }
        }
        piecePositions.clear();
    }
    
    void initializeBoard() {
        // Initialize white pieces
        placePiece(Position(7, 0), PieceFactory::createPiece(ROOK, WHITE));
        placePiece(Position(7, 1), PieceFactory::createPiece(KNIGHT, WHITE));
        placePiece(Position(7, 2), PieceFactory::createPiece(BISHOP, WHITE));
        placePiece(Position(7, 3), PieceFactory::createPiece(QUEEN, WHITE));
        placePiece(Position(7, 4), PieceFactory::createPiece(KING, WHITE));
        placePiece(Position(7, 5), PieceFactory::createPiece(BISHOP, WHITE));
        placePiece(Position(7, 6), PieceFactory::createPiece(KNIGHT, WHITE));
        placePiece(Position(7, 7), PieceFactory::createPiece(ROOK, WHITE));
        
        for (int i = 0; i < 8; i++) {
            placePiece(Position(6, i), PieceFactory::createPiece(PAWN, WHITE));
        }
        
        // Initialize black pieces
        placePiece(Position(0, 0), PieceFactory::createPiece(ROOK, BLACK));
        placePiece(Position(0, 1), PieceFactory::createPiece(KNIGHT, BLACK));
        placePiece(Position(0, 2), PieceFactory::createPiece(BISHOP, BLACK));
        placePiece(Position(0, 3), PieceFactory::createPiece(QUEEN, BLACK));
        placePiece(Position(0, 4), PieceFactory::createPiece(KING, BLACK));
        placePiece(Position(0, 5), PieceFactory::createPiece(BISHOP, BLACK));
        placePiece(Position(0, 6), PieceFactory::createPiece(KNIGHT, BLACK));
        placePiece(Position(0, 7), PieceFactory::createPiece(ROOK, BLACK));
        
        for (int i = 0; i < 8; i++) {
            placePiece(Position(1, i), PieceFactory::createPiece(PAWN, BLACK));
        }
    }
    
    void placePiece(Position pos, Piece* piece) {
        board[pos.getRow()][pos.getCol()] = piece;
        piecePositions[pos] = piece;
    }
    
    void removePiece(Position pos) {
        board[pos.getRow()][pos.getCol()] = nullptr;
        piecePositions.erase(pos);
    }
    
    Piece* getPiece(Position pos) {
        return board[pos.getRow()][pos.getCol()];
    }
    
    bool isOccupied(Position pos) {
        return getPiece(pos) != nullptr;
    }
    
    bool isOccupiedBySameColor(Position pos, Color color) {
        Piece* piece = getPiece(pos);
        return piece != nullptr && piece->getColor() == color;
    }
    
    void movePiece(Position from, Position to) {
        Piece* piece = getPiece(from);
        if (piece != nullptr) {
            // Remove captured piece if any
            Piece* capturedPiece = getPiece(to);
            if (capturedPiece != nullptr) {
                delete capturedPiece;
                piecePositions.erase(to);
            }
            
            // Move the piece
            board[from.getRow()][from.getCol()] = nullptr;
            board[to.getRow()][to.getCol()] = piece;
            
            // Update piece positions map
            piecePositions.erase(from);
            piecePositions[to] = piece;
            
            piece->setMoved(true);
        }
    }
    
    Position findKing(Color color) {
        for (auto& pair : piecePositions) {
            if (pair.second->getType() == KING && pair.second->getColor() == color) {
                return pair.first;
            }
        }
        return Position(-1, -1); // Invalid position if not found
    }
    
    vector<Position> getAllPiecesOfColor(Color color) {
        vector<Position> pieces;
        for (auto& pair : piecePositions) {
            if (pair.second->getColor() == color) {
                pieces.push_back(pair.first);
            }
        }
        return pieces;
    }

    void display() {
        constexpr int cellW = 3;  // cell width

        // — horizontal border —
        auto printBorder = [&]() {
            std::cout << "  +";
            for (int i = 0; i < 8; ++i)
                std::cout << std::string(cellW, '-') << "+";
            std::cout << "\n";
        };

        // — top border —
        printBorder();

        // — column labels inside the grid —
        std::cout << "  |";
        for (char f = 'a'; f <= 'h'; ++f) {
            int pad = (cellW - 1) / 2;
            std::cout
                << std::string(pad, ' ')
                << f
                << std::string(cellW - 1 - pad, ' ')
                << "|";
        }
        std::cout << "\n";

        // — border under labels —
        printBorder();

        // — each rank of pieces —
        for (int rank = 8; rank >= 1; --rank) {
            int row = 8 - rank;
            std::cout << rank << " |";

            for (int file = 0; file < 8; ++file) {
                Piece* p = board[row][file];
                std::string s = p ? p->toString() : "  ";  // two spaces if empty

                // center a 2-char string in cellW
                int pad = (cellW - 2) / 2;
                std::cout
                    << std::string(pad, ' ')
                    << s
                    << std::string(cellW - 2 - pad, ' ')
                    << "|";
            }

            std::cout << " " << rank << "\n";
            printBorder();
        }

        // — bottom labels inside the grid —
        std::cout << "  |";
        for (char f = 'a'; f <= 'h'; ++f) {
            int pad = (cellW - 1) / 2;
            std::cout
                << std::string(pad, ' ')
                << f
                << std::string(cellW - 1 - pad, ' ')
                << "|";
        }
        std::cout << "\n";

        // — final border —
        printBorder();
    }

};

// Now implement the piece movement methods
vector<Position> King::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int directions[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};
    
    for (int i = 0; i < 8; i++) {
        Position newPos(currentPos.getRow() + directions[i][0], currentPos.getCol() + directions[i][1]);
        if (newPos.isValid() && !board->isOccupiedBySameColor(newPos, this->color)) {
            moves.push_back(newPos);
        }
    }
    return moves;
}

vector<Position> Queen::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int directions[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1}};
    
    for (int d = 0; d < 8; d++) {
        for (int i = 1; i < 8; i++) {
            Position newPos(currentPos.getRow() + directions[d][0]*i, currentPos.getCol() + directions[d][1]*i);
            if (!newPos.isValid()) break;

            if (board->isOccupiedBySameColor(newPos, this->color)) break;

            moves.push_back(newPos);
            if (board->isOccupied(newPos)) break; // Stop after capturing
        }
    }
    return moves;
}

vector<Position> Rook::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            Position newPos(currentPos.getRow() + directions[d][0]*i, currentPos.getCol() + directions[d][1]*i);
            if (!newPos.isValid()) break;

            if (board->isOccupiedBySameColor(newPos, this->color)) break;

            moves.push_back(newPos);
            if (board->isOccupied(newPos)) break;
        }
    }
    return moves;
}

vector<Position> Bishop::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int directions[4][2] = {{-1,-1}, {-1,1}, {1,-1}, {1,1}};
    
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            Position newPos(currentPos.getRow() + directions[d][0]*i, currentPos.getCol() + directions[d][1]*i);
            if (!newPos.isValid()) break;
            if (board->isOccupiedBySameColor(newPos, this->color)) break;
            moves.push_back(newPos);
            if (board->isOccupied(newPos)) break;
        }
    }
    return moves;
}

vector<Position> Knight::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int knightMoves[8][2] = {{-2,-1}, {-2,1}, {-1,-2}, {-1,2}, {1,-2}, {1,2}, {2,-1}, {2,1}};
    
    for (int i = 0; i < 8; i++) {
        Position newPos(currentPos.getRow() + knightMoves[i][0], currentPos.getCol() + knightMoves[i][1]);
        if (newPos.isValid() && !board->isOccupiedBySameColor(newPos, this->color)) {
            moves.push_back(newPos);
        }
    }
    return moves;
}

vector<Position> Pawn::getPossibleMoves(Position currentPos, Board* board) {
    vector<Position> moves;
    int direction = (color == WHITE) ? -1 : 1;
    
    // Forward move
    Position oneStep(currentPos.getRow() + direction, currentPos.getCol());
    if (oneStep.isValid() && !board->isOccupied(oneStep)) {
        moves.push_back(oneStep);
        
        // Double move from starting position
        if (!hasMoved) {
            Position twoStep(currentPos.getRow() + 2*direction, currentPos.getCol());
            if (twoStep.isValid() && !board->isOccupied(twoStep)) {
                moves.push_back(twoStep);
            }
        }
    }
    
    // Diagonal captures
    Position leftCapture(currentPos.getRow() + direction, currentPos.getCol() - 1);
    Position rightCapture(currentPos.getRow() + direction, currentPos.getCol() + 1);
    
    if (leftCapture.isValid() && board->isOccupied(leftCapture) && 
        !board->isOccupiedBySameColor(leftCapture, this->color)) {
        moves.push_back(leftCapture);
    }
    
    if (rightCapture.isValid() && board->isOccupied(rightCapture) && 
        !board->isOccupiedBySameColor(rightCapture, this->color)) {
        moves.push_back(rightCapture);
    }
    
    return moves;
}

// Chess Rules class - Strategy Pattern for game rules
class ChessRules {
public:
    virtual ~ChessRules() {}
    virtual bool isValidMove(Move move, Board* board) = 0;
    virtual bool isInCheck(Color color, Board* board) = 0;
    virtual bool isCheckmate(Color color, Board* board) = 0;
    virtual bool isStalemate(Color color, Board* board) = 0;
    virtual bool wouldMoveCauseCheck(Move move, Board* board, Color kingColor) = 0;
};

class StandardChessRules : public ChessRules {
public:
    bool isValidMove(Move move, Board* board) override {
        Piece* piece = move.getPiece();
        vector<Position> possibleMoves = piece->getPossibleMoves(move.getFrom(), board);
        
        // Check if the target position is in possible moves
        bool validDestination = false;
        for (const Position& pos : possibleMoves) {
            if (pos == move.getTo()) {
                validDestination = true;
                break;
            }
        }
        
        if (!validDestination) {
            return false;
        }
        
        // Check if move would put own king in check
        return !wouldMoveCauseCheck(move, board, piece->getColor());
    }
    
    bool wouldMoveCauseCheck(Move move, Board* board, Color kingColor) override {
        // Create a temporary copy to simulate the move safely
        Piece* movingPiece = board->getPiece(move.getFrom());
        Piece* capturedPiece = board->getPiece(move.getTo());
        
        if (movingPiece == nullptr) return true; // Invalid move
        
        // Temporarily execute the move
        board->removePiece(move.getFrom());
        if (capturedPiece != nullptr) {
            board->removePiece(move.getTo());
        }
        board->placePiece(move.getTo(), movingPiece);
        
        // Check if king is in check after the move
        bool inCheck = isInCheck(kingColor, board);
        
        // Undo the move
        board->removePiece(move.getTo());
        board->placePiece(move.getFrom(), movingPiece);
        if (capturedPiece != nullptr) {
            board->placePiece(move.getTo(), capturedPiece);
        }
        
        return inCheck;
    }
    
    bool isInCheck(Color color, Board* board) override {
        Position kingPos = board->findKing(color);
        if (kingPos.getRow() == -1) return false; // King not found
        
        Color opponentColor = (color == WHITE) ? BLACK : WHITE;
        vector<Position> opponentPieces = board->getAllPiecesOfColor(opponentColor);
        
        for (const Position& pos : opponentPieces) {
            Piece* piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            for (const Position& targetPos : moves) {
                if (targetPos == kingPos) {
                    return true;
                }
            }
        }
        return false;
    }
    
    bool isCheckmate(Color color, Board* board) override {
        if (!isInCheck(color, board)) return false;
        
        vector<Position> pieces = board->getAllPiecesOfColor(color);
        for (const Position& pos : pieces) {
            Piece* piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            
            for (const Position& targetPos : moves) {
                Move move(pos, targetPos, piece, board->getPiece(targetPos));
                if (isValidMove(move, board)) {
                    return false; // Found a valid move, not checkmate
                }
            }
        }
        return true;
    }
    
    bool isStalemate(Color color, Board* board) override {
        if (isInCheck(color, board)) return false;
        
        vector<Position> pieces = board->getAllPiecesOfColor(color);
        for (const Position& pos : pieces) {
            Piece* piece = board->getPiece(pos);
            vector<Position> moves = piece->getPossibleMoves(pos, board);
            
            for (const Position& targetPos : moves) {
                Move move(pos, targetPos, piece, board->getPiece(targetPos));
                if (isValidMove(move, board)) {
                    return false; // Found a valid move, not stalemate
                }
            }
        }
        return true;
    }
};

// Message class for chat functionality
class Message {
private:
    string senderId;
    string content;
    time_t timestamp;

public:
    Message(string sId, string msg) {
        senderId = sId;
        content = msg;
        timestamp = time(0);
    }
    
    string getSenderId() const { 
        return senderId; 
    }
    string getContent() const { 
        return content; 
    }
    time_t getTimestamp() const { 
        return timestamp; 
    }
    
    string toString() const {
        return "[" + senderId + "]: " + content;
    }
};

// Mediator Pattern - Interface
class ChatMediator {
public:
    virtual ~ChatMediator() {}
    virtual void sendMessage(Message* message, User* user) = 0;
    virtual void addUser(User* user) = 0;
    virtual void removeUser(User* user) = 0;
};

// Colleague interface for Mediator Pattern
class Colleague {
protected:
    ChatMediator* mediator;
    
public:
    Colleague() : mediator(nullptr) {}
    virtual ~Colleague() {}
    virtual void send(Message* message) = 0;
    virtual void receive(Message* message) = 0;
    virtual void setMediator(ChatMediator* med) { 
        mediator = med; 
    }
    ChatMediator* getMediator() const { 
        return mediator; 
    }
};

// User class now inheriting from Colleague for proper Mediator Pattern
class User : public Colleague {
private:
    string id;
    string name;
    int score;

public:
    User(string userId, string userName) : Colleague() {
        id = userId;
        name = userName;
        score = 1000; // Starting score
    }
    
    string getId() const { 
        return id; 
    }
    string getName() const { 
        return name; 
    }
    int getScore() const { 
        return score; 
    }
    
    void incrementScore(int points) {
        score += points;
    }
    
    void decrementScore(int points) {
        score -= points;
    }
    
    string toString() const {
        return name + " (Score: " + to_string(score) + ")";
    }
    
    // Implement Colleague interface
    void send(Message* message) override {
        if (mediator != nullptr) {
            mediator->sendMessage(message, this);
        }
    }
    
    void receive(Message* message) override {
        cout << "User " << name << " received message from " << message->getSenderId() << ": " << message->getContent() << endl;
    }
};

// Match class implementing Mediator Pattern
class Match : public ChatMediator {
private:
    string matchId;
    User* whitePlayer;
    User* blackPlayer;
    Board* board;
    ChessRules* rules;
    Color currentTurn;
    GameStatus status;
    vector<Move> moveHistory;
    vector<Message*> chatHistory;

public:
    Match(string mId, User* white, User* black) {
        matchId = mId;
        whitePlayer = white;
        blackPlayer = black;
        board = new Board();
        rules = new StandardChessRules();
        currentTurn = WHITE;
        status = IN_PROGRESS;
        
        // Set mediator for both users
        whitePlayer->setMediator(this);
        blackPlayer->setMediator(this);
        
        cout << "Match started between " << whitePlayer->getName() << " (White) and " 
             << blackPlayer->getName() << " (Black)" << endl;
    }
    
    ~Match() {
        delete board;
        delete rules;
    }
    
    bool makeMove(Position from, Position to, User* player) {
        if (status != IN_PROGRESS) {
            cout << "Game is not in progress!" << endl;
            return false;
        }
        
        Color playerColor = getPlayerColor(player);
        if (playerColor != currentTurn) {
            cout << "It's not your turn!" << endl;
            return false;
        }
        
        Piece* piece = board->getPiece(from);
        if (piece == nullptr || piece->getColor() != playerColor) {
            cout << "Invalid piece selection!" << endl;
            return false;
        }
        
        Move move(from, to, piece, board->getPiece(to));
        
        if (!rules->isValidMove(move, board)) {
            cout << "Invalid move!" << endl;
            return false;
        }
        
        // Execute move
        board->movePiece(from, to);
        moveHistory.push_back(move);
        
        cout << player->getName() << " moved " << piece->getSymbol() 
             << " from " << from.toChessNotation() << " to " << to.toChessNotation() << endl;
        
        board->display();
        
        // Check game end conditions
        Color opponentColor = (currentTurn == WHITE) ? BLACK : WHITE;
        if (rules->isCheckmate(opponentColor, board)) {
            endGame(player, "checkmate");
            return true;
        } 
        else if (rules->isStalemate(opponentColor, board)) {
            endGame(player, "stalemate");
            return true;
        } 
        else {
            currentTurn = opponentColor;
            if (rules->isInCheck(opponentColor, board)) {
                cout << getPlayerByColor(opponentColor)->getName() << " is in check!" << endl;
            }
        }
        
        return true;
    }
    
    void quitGame(User* player) {
        User* opponent = (player == whitePlayer) ? blackPlayer : whitePlayer;
        endGame(opponent, "quit");
        player->decrementScore(50); // Penalty for quitting
        cout << player->getName() << " quit the game. Score decreased by 50." << endl;
    }
    
    void endGame(User* winner, string reason) {
        status = COMPLETED;
        
        if (winner != nullptr) {
            User* loser = (winner == whitePlayer) ? blackPlayer : whitePlayer;
            winner->incrementScore(30);
            loser->decrementScore(20);
            cout << "Game ended - " << winner->getName() << " wins by " << reason << "!" << endl;
            cout << "Score update: " << winner->getName() << " +30, " << loser->getName() << " -20" << endl;
        } 
        else {
            cout << "Game ended in " << reason << "! No score change." << endl;
        }
    }
    
    Color getPlayerColor(User* player) {
        return (player == whitePlayer) ? WHITE : BLACK;
    }
    
    User* getPlayerByColor(Color color) {
        return (color == WHITE) ? whitePlayer : blackPlayer;
    }
    
    // Mediator Pattern implementation
    void sendMessage(Message* message, User* user) override {
        chatHistory.push_back(message);
        
        User* recipient = (user == whitePlayer) ? blackPlayer : whitePlayer;
        recipient->receive(message);
        cout << "Chat in match " << matchId << " - " << message->getContent() << endl;
    }
    
    void addUser(User* user) override {
        // Not applicable for chess match (always 2 users)
    }
    
    void removeUser(User* user) override {
        quitGame(user);
    }
    
    string getMatchId() const { 
        return matchId; 
    }
    GameStatus getStatus() const { 
        return status; 
    }
    User* getWhitePlayer() const { 
        return whitePlayer; 
    }
    User* getBlackPlayer() const { 
        return blackPlayer; 
    }
    Board* getBoard() const { 
        return board; 
    }
};

// Matching Strategy interface
class MatchingStrategy {
public:
    virtual ~MatchingStrategy() {}
    virtual User* findMatch(User* user, vector<User*>& waitingUsers) = 0;
};

// Score-based matching strategy
class ScoreBasedMatching : public MatchingStrategy {
private:
    int scoreTolerance;

public:
    ScoreBasedMatching(int tolerance) {
        scoreTolerance = tolerance;
    }
    
    User* findMatch(User* user, vector<User*>& waitingUsers) override {
        User* bestMatch = nullptr;
        int bestScoreDiff = INT_MAX;
        
        for (User* waitingUser : waitingUsers) {
            if (waitingUser->getId() != user->getId()) {
                int scoreDiff = abs(waitingUser->getScore() - user->getScore());
                if (scoreDiff <= scoreTolerance && scoreDiff < bestScoreDiff) {
                    bestMatch = waitingUser;
                    bestScoreDiff = scoreDiff;
                }
            }
        }
        return bestMatch;
    }
};

// Game Manager - Singleton Pattern
class GameManager {
private:
    static GameManager* instance;
    map<string, Match*> activeMatches; // matchId --> Match
    vector<User*> waitingUsers;
    MatchingStrategy* matchingStrategy;
    int matchCounter;
    
    GameManager() {
        matchingStrategy = new ScoreBasedMatching(100); // 100 points tolerance
        matchCounter = 0;
    }

public:
    static GameManager* getInstance() {
        if (instance == nullptr) {
            instance = new GameManager();
        }
        return instance;
    }
    
    ~GameManager() {
        delete matchingStrategy;
        for (auto& pair : activeMatches) {
            delete pair.second;
        }
    }
    
    void requestMatch(User* user) {
        cout << user->getName() << " is looking for a match..." << endl;
        
        User* opponent = matchingStrategy->findMatch(user, waitingUsers);
        
        if (opponent != nullptr) {
            // Remove opponent from waiting list
            waitingUsers.erase(remove(waitingUsers.begin(), waitingUsers.end(), opponent), waitingUsers.end());
            
            string matchId = "MATCH_" + to_string(++matchCounter);
            Match* match = new Match(matchId, user, opponent);
            activeMatches[matchId] = match;
            
            cout << "Match found! " << user->getName() << " vs " << opponent->getName() << endl;
            match->getBoard()->display();
        } 
        else {
            waitingUsers.push_back(user);
            cout << user->getName() << " added to waiting list." << endl;
        }
    }
    
    void makeMove(string matchId, Position from, Position to, User* player) {
        if (activeMatches.find(matchId) != activeMatches.end()) {
            Match* match = activeMatches[matchId];
            match->makeMove(from, to, player);
            
            if (match->getStatus() == COMPLETED) {
                delete match;
                activeMatches.erase(matchId);
                cout << "Match " << matchId << " completed and removed from active matches." << endl;
            }
        }
    }
    
    void quitMatch(string matchId, User* player) {
        if (activeMatches.find(matchId) != activeMatches.end()) {
            Match* match = activeMatches[matchId];
            match->quitGame(player);
            delete match;
            activeMatches.erase(matchId);
        }
    }
    
    void sendChatMessage(string matchId, string message, User* user) {
        if (activeMatches.find(matchId) != activeMatches.end()) {
            Match* match = activeMatches[matchId];
            Message* msg = new Message(user->getId(), message);
            match->sendMessage(msg, user);
        }
    }
    
    Match* getMatch(string matchId) {
        if (activeMatches.find(matchId) != activeMatches.end()) {
            return activeMatches[matchId];
        }
        return nullptr;
    }
    
    void displayActiveMatches() {
        cout << "\n=== Active Matches ===" << endl;
        for (auto& pair : activeMatches) {
            Match* match = pair.second;
            cout << "Match " << match->getMatchId() << ": " 
                 << match->getWhitePlayer()->getName() << " vs " 
                 << match->getBlackPlayer()->getName() << endl;
        }
        cout << "Total active matches: " << activeMatches.size() << endl;
        cout << "Users waiting: " << waitingUsers.size() << endl;
    }
};

// Initialize static member
GameManager* GameManager::instance = nullptr;

// Util class for basic demo
class ChessSystemDemo {
public:
    // Method to demonstrate Scholar's Mate (4-move checkmate)
    static void demonstrateScholarsMate() {
        cout << "\n=== Scholar's Mate Demo (4-move checkmate) ===" << endl;
        
        User* samarth = new User("DEMO_1", "Samarth");
        User* ansh = new User("DEMO_2", "Ansh");
        
        Match* demoMatch = new Match("DEMO_MATCH", samarth, ansh);
        demoMatch->getBoard()->display();
        
        // Proper Scholar's Mate sequence with correct coordinates
        cout << "\nMove 1: White e2-e4" << endl;
        demoMatch->makeMove(Position(6, 4), Position(4, 4), samarth); // e2-e4
        
        cout << "\nMove 1: Black e7-e5" << endl;
        demoMatch->makeMove(Position(1, 4), Position(3, 4), ansh); // e7-e5
        
        cout << "\nMove 2: White Bf1-c4 (targeting f7)" << endl;
        demoMatch->makeMove(Position(7, 5), Position(4, 2), samarth); // Bf1-c4
        
        cout << "\nMove 2: Black Nb8-c6 (developing)" << endl;
        demoMatch->makeMove(Position(0, 1), Position(2, 2), ansh); // Nb8-c6
        
        cout << "\nMove 3: White Qd1-h5 (attacking f7 and h7)" << endl;
        demoMatch->makeMove(Position(7, 3), Position(3, 7), samarth); // Qd1-h5 (row 3, col 7 = h5)
        
        cout << "\nMove 3: Black Ng8-f6?? (defending h7 but exposing f7)" << endl;
        demoMatch->makeMove(Position(0, 6), Position(2, 5), ansh); // Ng8-f6
        
        cout << "\nMove 4: White Qh5xf7# (Checkmate!)" << endl;
        bool gameEnded = demoMatch->makeMove(Position(3, 7), Position(1, 5), samarth); // Qh5xf7#
        
        if (demoMatch->getStatus() != COMPLETED) {
            cout << "Note: Checkmate detection may need refinement for this position." << endl;
        }
        
        // Demonstrate chat functionality
        cout << "\n=== Testing Chat Functionality ===" << endl;
        samarth->send(new Message(samarth->getId(), "Good game!"));
        ansh->send(new Message(ansh->getId(), "Thanks, that was a quick one!"));
        
        // Clean up properly
        delete demoMatch; // This will handle board and rules cleanup
        delete samarth;
        delete ansh;
    }
};

// Main function to run the chess system
int main() {
    cout << "=== Chess System with Design Patterns Demo ===" << endl;
    
    // Test Scholar's Mate
    ChessSystemDemo::demonstrateScholarsMate();
    
    // Demonstrate Game Manager functionality
    cout << "\n=== Game Manager Demo ===" << endl;
    GameManager* gm = GameManager::getInstance();
    
    User* samarth = new User("USER_1", "Samarth");
    User* maulik = new User("USER_2", "Maulik");
    User* ansh = new User("USER_3", "Ansh");
    
    cout << "\nUsers: " << samarth->toString() << ", " << maulik->toString() << ", " << ansh->toString() << endl;
    
    // Request matches
    gm->requestMatch(samarth);
    gm->requestMatch(maulik);  // Should create a match
    gm->requestMatch(ansh); // Should go to waiting list
    
    gm->displayActiveMatches();
    
    // Clean up
    delete samarth;
    delete maulik;
    delete ansh;
    
    // Clean up singleton instance
    delete GameManager::getInstance();    
    return 0;
}