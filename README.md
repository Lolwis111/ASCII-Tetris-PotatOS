# ASCII Tetris game

Ported to PotatOS.
Copy tetris.c into kernel/src/ and tetris.h into kernel/include/

Call run_tetris(width, height) from the main() in kernel.c.



Controls:

    q: 		left
    s: 		bottom (quicker)
    d: 		right
    [space]: 	rotate

What it looks like:

    [LEVEL: 1 | SCORE: 0]
    ~~~~~~~~~~~~~~~~~~~~~~~~~
    !                        !
    !                        !
    !                        !
    !                        !
    !          O O           !
    !          O             !
    !          O             !
    !                        !
    !                        !
    !                        !
    !                        !
    !              &         !
    !              &         !
    !      # #   X & &       !
    !      # # X X X @ @ @ @ !
    ~~~~~~~~~~~~~~~~~~~~~~~~~

# License

This is under MIT License, please read the LICENSE file for further details.
Do not hesitate to fork this repository and customize it, enjoy!
