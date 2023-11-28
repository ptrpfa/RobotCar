""" 
  Testcase 0: 3 3
  Testcase 1: 4 3
  Testcase 2: 5 3
  Testcase 3: 2 3

"""

# this function checks if a queen can be placed in location (col,row)

# temp function
def printBoard(board):
  for row in board:
      print(row)

def isValid(col,row,chessBoard):
  # Check row
  if(1 in chessBoard[row]):
    print("row")
    return False
  # Check column
  for row_index in range(row):
    if(chessBoard[row_index][col] == 1):
      print("col")
      return False
  """ Check diagonals (assume there are no queens in rows below chosen position) """
  # Check top left diagonal
  col_index = col - 1
  for row_index in range(row - 1, -1, -1):
    if(col_index >= 0 and chessBoard[row_index][col_index] == 1):
        print("top left diagonal")
        return False
    col_index -= 1

  # Check top right diagonal
  col_index = col + 1
  for row_index in range(row - 1, -1, -1):
    if(col_index != len(chessBoard) and chessBoard[row_index][col_index] == 1):
      print("top right diagonal")
      return False
    col_index += 1

  # Only return True once all checks are passed
  return True

# initialize chessboard (8 x 8)
N = 8
chessBoard = []
for i in range(N):
  chessBoard.append([])
  for j in range(N):
    chessBoard[i].append(0)

# place a queen in the upper left corner of the board
chessBoard[1][1]=1
chessBoard[0][4]=1
chessBoard[2][6]=1

columnIndex = int(input("Enter a column index:\n"))
rowIndex = int(input("Enter a row index:\n"))

print("Placing a queen at column", columnIndex,"row", rowIndex,":", isValid(columnIndex,rowIndex,chessBoard))