    /* awk */
    switch (fork()) {
        case -1:
            exitmsg("fork awk");

        case 0:
            if (close(tr_to_awk[1]) == -1)
                _exitmsg("close 1 awk");
            
            if (tr_to_awk[0] != STDIN_FILENO) {
                if (dup2(tr_to_awk[0], STDIN_FILENO) == -1)
                    _exitmsg("dup2 1 awk");
                if (close(tr_to_awk[0]) == -1)
                    _exitmsg("close 72 awk");
            }

            if (close(awk_to_sort[0]) == -1)
                _exitmsg("close awk2sort 0");

            if (awk_to_sort[1] != STDOUT_FILENO) {
                if (dup2(awk_to_sort[1], STDOUT_FILENO) == -1)
                    _exitmsg("dup2 1 awk");
                if (close(awk_to_sort[1]) == -1)
                    _exitmsg("close 72 awk");
            }

            execlp("awk", "awk",
                   "{ for(i = 1; i <= NF; i++) { print $i; } }",
                   (char *) NULL);
            _exitmsg("execlp awk"); /* Reads from pipe */

        default:
            break;
    }
