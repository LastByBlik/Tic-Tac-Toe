#include <algorithm>
#include <cctype>
#include <ctime>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

const   char    SPACE_SYMB = ' ';

typedef std::default_random_engine      T_gen_type;
typedef std::string                     T_str;
typedef std::vector     < T_str     >   T_strings;
typedef std::set        < T_str     >   T_strings_set;
typedef T_strings                       T_field_matr;
typedef T_str                           T_position_str_min;
typedef T_strings                       T_position_strings;

typedef std::map
<
    T_position_str_min,
    T_position_strings
>
T_position_strings_of_position_str_min;

enum    T_game_res
{
    GAME_RES_EMPTY,
    GAME_RES_LOSS,
    GAME_RES_DRAW,
    GAME_RES_WIN
};

T_game_res  operator!   (T_game_res    game_res)
{
    switch (game_res)
    {
    case    GAME_RES_EMPTY:   return  GAME_RES_EMPTY;
    case    GAME_RES_LOSS:   return  GAME_RES_WIN;
    case    GAME_RES_DRAW:   return  GAME_RES_DRAW;
    case    GAME_RES_WIN:   return  GAME_RES_LOSS;
    default:   return  GAME_RES_EMPTY;
    }
}

struct  T_position
{

    T_str                   field_str_;
    bool                    made_computer_;
    mutable     T_game_res  res_for_computer_;
    mutable     bool        is_final_;

    T_position
    (
        T_str   const& field_str,
        bool                computer_symb_is_first
    )
        :
        field_str_{ field_str },
        res_for_computer_{},
        is_final_{}
    {
        int     spaces_count = std::count
        (
            field_str.begin(),
            field_str.end(),
            SPACE_SYMB
        );

        bool    spaces_count_is_even = spaces_count % 2 == 0;

        made_computer_ = spaces_count_is_even
            == computer_symb_is_first;

        if (
            spaces_count
            == 0
            )
        {
            is_final_ = true;
            res_for_computer_ = GAME_RES_DRAW;
        }
    }

    bool    operator<   (T_position    const& position)                  const
    {
        return      std::make_pair
        (
            field_str_,
            made_computer_
        )

            < std::make_pair
            (
                position.field_str_,
                position.made_computer_
            );
    }

};

typedef std::set    < T_position    >   T_positions;
typedef std::set    < T_game_res    >   T_results_set_for_computer;

class   T_xs_and_os
{

    static  const   int     FIELD_DIM = 3;

    static  const   int     FIELD_SIZE = FIELD_DIM
        * FIELD_DIM;

    static  const   int     FIRST_CELL_NUM = 1;
    static  const   int     LAST_CELL_NUM = FIELD_SIZE;

    static  const   char    GAME_SYMB_FIRST = 'X';
    static  const   char    GAME_SYMB_SECOND = 'O';
    static  const   char    ZERO_SYMB = '0';

    static  const   char    MINUS_SYMB = '-';
    static  const   char    VERTICAL_LINE_SYMB = '|';
    static  const   int     SQUARE_CORNERS_TOTAL = 4;

    bool                    computer_symb_is_first_;
    char                    computer_symb_;
    char                    player_symb_;
    T_positions             positions_;

public:

    T_xs_and_os()
        :
        computer_symb_is_first_{},
        computer_symb_{},
        player_symb_{}
    {}

    void    input_player_symb()
    {
        const   char    YES_SYMB = 'Y';
        const   char    NO_SYMB = 'N';

        std::cout << "\n\n\n\n\n"
            << "Want to play with "
            << GAME_SYMB_FIRST
            << " ("
            << YES_SYMB
            << "/"
            << NO_SYMB
            << "): ";

        char    YN_symb_for_player_symb_is_first{};
        std::cin >> YN_symb_for_player_symb_is_first;

        computer_symb_is_first_ = std::toupper(YN_symb_for_player_symb_is_first)
            != std::toupper(YES_SYMB);

        computer_symb_ = computer_symb_is_first_
            ? GAME_SYMB_FIRST
            : GAME_SYMB_SECOND;

        player_symb_ = computer_symb_is_first_
            ? GAME_SYMB_SECOND
            : GAME_SYMB_FIRST;
    }

    void    play()
    {
        T_str   field_str_cur
        (
            FIELD_SIZE,
            SPACE_SYMB
        );

        for (
            bool
            cur_symb_is_first{ true };
            !field_str_is_final(field_str_cur);
            cur_symb_is_first = !cur_symb_is_first
            )
        {
            bool    is_computer_move = computer_symb_is_first_
                == cur_symb_is_first;

            if (is_computer_move)
            {
                if (
                    !successfully_make_computer_move(field_str_cur)
                    )
                {
                    return;
                }
            }
            else
            {
                input_player_move(field_str_cur);
            }
        }

        print_field(field_str_cur);
        print_game_res_for_player(field_str_cur);
    }

private:

    bool    field_str_is_final(T_str   const& field_str)
    {
        return  get_position_for_field_str(field_str).is_final_;
    }

    static
        void    print_field(T_str   const& field_str)
    {
        T_field_matr    field_matr;

        set_field_matr
        (
            field_str,
            field_matr
        );

        print_field_matr(field_matr);
    }

    static
        void    print_field_matr(T_field_matr  const& field_matr)
    {
        for (size_t i{}; i < field_matr.size(); ++i)
        {
            print_field_row_of_matr_with_ind
            (
                field_matr,
                i
            );
        }
    }

    static
        void    print_field_row_of_matr_with_ind
        (
            T_field_matr    const& field_matr,
            int                         i
        )
    {
        const   int     SPACES_COUNT_BETWEEN_CELL_BORDER_AND_SYMBOL = 1;

        int             border_horiz_len

            =
            (SPACES_COUNT_BETWEEN_CELL_BORDER_AND_SYMBOL + 1)
            * 2
            * field_matr.size()
            + 1;

        T_str   border_horiz = T_str(
            border_horiz_len,
            MINUS_SYMB
        );

        if (i == 0)
        {
            std::cout << border_horiz
                << std::endl;
        }

        auto& field_row = field_matr[i];

        for (size_t j{}; j < field_row.size(); ++j)
        {
            print_field_cell_of_row_with_ind
            (
                field_row,
                j,
                SPACES_COUNT_BETWEEN_CELL_BORDER_AND_SYMBOL
            );
        }

        std::cout << std::endl
            << border_horiz
            << std::endl;
    }

    static
        void    print_field_cell_of_row_with_ind
        (
            T_str   const& field_row,
            int                 j,
            size_t              spaces_count_between_cell_border_and_symbol
        )
    {
        T_str   spaces_line(
            spaces_count_between_cell_border_and_symbol,
            SPACE_SYMB
        );

        if (j == 0)
        {
            std::cout << VERTICAL_LINE_SYMB;
        }

        std::cout << spaces_line
            << field_row[j]
            << spaces_line
            << VERTICAL_LINE_SYMB;
    }

    void    print_game_res_for_player(T_str    const& field_str)
    {
        auto    res_for_player
            = !get_position_for_field_str(field_str).res_for_computer_;

        std::cout << game_res_message_for_player(res_for_player)
            << std::endl;
    }

    T_str   game_res_message_for_player(T_game_res     res_for_player)
    {
        switch (res_for_player)
        {
        case    GAME_RES_WIN:   return  "You won.";
        case    GAME_RES_DRAW:   return  "Draw.";
        case    GAME_RES_LOSS:   return  "You lost.";
        default:   return  "Game result is unknown.";
        }
    }

    static
        void    set_field_matr
        (
            T_str           const& field_str,
            T_field_matr& field_matr
        )
    {
        for (size_t i{}; i < FIELD_DIM; ++i)
        {
            field_matr.push_back
            (
                field_str.substr
                (
                    FIELD_DIM * i,
                    FIELD_DIM
                )
            );
        }
    }

    static
        bool    symb_won_in_matr
        (
            char                        game_symb,
            T_field_matr    const& field_matr
        )
    {
        T_field_matr    rotated_matr{ field_matr };
        transp(rotated_matr);
        reverse_strings(rotated_matr);

        return      game_symb_won_in_row_or_main_diag_of_matr
        (
            game_symb,
            field_matr
        )

            || game_symb_won_in_row_or_main_diag_of_matr
            (
                game_symb,
                rotated_matr
            );
    }

    static
        bool    game_symb_won_in_row_or_main_diag_of_matr
        (
            char                        game_symb,
            T_field_matr    const& field_matr
        )
    {
        T_str   winning_str
        {
            T_str(
                        FIELD_DIM,
                        game_symb
                    )
        };

        return          std::find
        (
            field_matr.begin(),
            field_matr.end(),
            winning_str
        )

            != field_matr.end()

            || winning_str

            == T_str{
                            field_matr[0][0],
                            field_matr[1][1],
                            field_matr[2][2]
        };
    }

    void   input_player_move(T_str& field_str)
    {
        print_field(field_str);
        int     ind{};

        do
        {
            input_field_str_ind(ind);
        } while (
            field_str[ind]
            != SPACE_SYMB
            );

        field_str[ind] = player_symb_;
    }

    void    input_field_str_ind(int& ind)
    {
        int     move_cell_num{};
        char    digit_symb{};

        for (;;)
        {
            print_player_move_prompt();
            std::cin >> digit_symb;

            if (
                std::isdigit(digit_symb)
                )
            {
                move_cell_num = digit_symb
                    - ZERO_SYMB;

                if (
                    move_cell_num >= FIRST_CELL_NUM
                    && move_cell_num <= LAST_CELL_NUM
                    )
                {
                    break;
                }
            }
        }


        int     ind_from_num = move_cell_num
            - FIRST_CELL_NUM;

        int     i_from_num = ind_from_num
            / FIELD_DIM;

        int     i = FIELD_DIM
            - 1
            - i_from_num;

        int     j = ind_from_num
            % FIELD_DIM;

        ind = FIELD_DIM * i + j;
    }

    void    print_player_move_prompt()                                          const
    {
        std::cout << std::endl
            << "Your move by symbol "
            << player_symb_
            << "."
            << std::endl
            << "Enter cell number "
            << FIRST_CELL_NUM
            << ".."
            << LAST_CELL_NUM
            << " on numeric keypad: ";
    }

    bool    successfully_make_computer_move(T_str& field_str)
    {
        bool    bool_res = false;

        T_position_strings_of_position_str_min
            win_position_strings_of_position_str_min;

        T_position_strings_of_position_str_min
            draw_position_strings_of_position_str_min;

        for (size_t i{}; i < field_str.size(); ++i)
        {
            if (
                field_str[i]
                != SPACE_SYMB
                )
            {
                continue;
            }

            T_str       field_str_cur{ field_str };
            field_str_cur[i] = computer_symb_;

            auto    position_cur = get_position_for_field_str(field_str_cur);

            switch (position_cur.res_for_computer_)
            {
            case    GAME_RES_WIN:
                win_position_strings_of_position_str_min
                    [
                        get_position_str_min(field_str_cur)
                    ]
                .push_back(field_str_cur);

                bool_res = true;
                break;

            case    GAME_RES_DRAW:
                draw_position_strings_of_position_str_min
                    [
                        get_position_str_min(field_str_cur)
                    ]
                .push_back(field_str_cur);

                bool_res = true;
                break;

            default:
                break;
            }
        }

        if (bool_res)
        {
            set_rand_field_str
            (
                !win_position_strings_of_position_str_min.empty()
                ? win_position_strings_of_position_str_min
                : draw_position_strings_of_position_str_min,

                field_str
            );
        }
        else
        {
            print_field(field_str);

            std::cout << "I give up!"
                << std::endl;
        }

        return  bool_res;
    }

    T_str   get_position_str_min(T_str     const& field_str)
    {
        T_strings_set   field_strings_set;
        T_field_matr    field_matr;

        set_field_matr
        (
            field_str,
            field_matr
        );

        for (int i{}; ; ++i)
        {
            field_strings_set.insert
            (
                get_field_str_of_fild_matr(field_matr)
            );

            if (
                i
                == SQUARE_CORNERS_TOTAL * 2 - 1
                )
            {
                break;
            }

            i % 2 == 0
                ? transp(field_matr)
                : reverse_strings(field_matr);
        }

        return  *field_strings_set.begin();
    }

    T_str   get_field_str_of_fild_matr(T_field_matr& field_matr)
    {
        T_str   res_str;

        for (
            auto& matr_row :
            field_matr
            )
        {
            res_str += matr_row;
        }

        return  res_str;
    }

    T_position  get_position_for_field_str(T_str     const& field_str)
    {
        return  get_or_calc_position
        (
            T_position
            (
                field_str,
                computer_symb_is_first_
            )
        );
    }

    T_position  get_or_calc_position(T_position    const& position)
    {
        auto    it = positions_.find(position);

        if (
            it != positions_.end()
            )
        {
            return  *it;
        }

        fill_position(position);
        positions_.insert(position);
        return  position;
    }

    void    fill_position(T_position   const& position)
    {
        calc_is_final_of_position(position);

        if (position.is_final_)
        {
            return;
        }

        T_results_set_for_computer  children_results_for_computer;

        calc_children_and_set_their_results_for_computer
        (
            position,
            children_results_for_computer
        );

        position.res_for_computer_ = position.made_computer_
            ? *children_results_for_computer.begin()
            : *children_results_for_computer.rbegin();
    }

    void    calc_is_final_of_position(T_position   const& position)
    {
        auto    cur_game_symb = position.made_computer_
            ? computer_symb_
            : player_symb_;

        if (
            symb_won_in_field_str
            (
                cur_game_symb,
                position.field_str_
            )
            )
        {
            position.is_final_ = true;

            position.res_for_computer_ = position.made_computer_
                ? GAME_RES_WIN
                : GAME_RES_LOSS;
        }
    }

    bool    symb_won_in_field_str
    (
        char                symb,
        T_str   const& field_str
    )
    {
        T_field_matr    field_matr;

        set_field_matr
        (
            field_str,
            field_matr
        );

        return  symb_won_in_matr
        (
            symb,
            field_matr
        );
    }

    static
        void    set_rand_field_str
        (
            T_position_strings_of_position_str_min   const&
            position_strings_of_position_str_min,

            T_str& field_str
        )
    {
        auto    rand_position_strings = get_rand_val_of_cont
        (
            position_strings_of_position_str_min
        )
            .second;

        field_str = get_rand_val_of_cont
        (
            rand_position_strings
        );
    }

    template< typename    TT_cont >
    static
        typename    TT_cont::value_type     get_rand_val_of_cont
        (
            TT_cont     const& cont
        )
    {
        static  T_gen_type                          gen_(unsigned(time(0)));

        std::uniform_int_distribution< size_t >     cont_ind_distr
        (
            0,
            cont.size() - 1
        );

        auto    it = cont.begin();
        size_t  it_shift = cont_ind_distr(gen_);

        std::advance
        (
            it,
            it_shift
        );

        return  *it;
    }

    static
        void    transp(T_field_matr& field_matr)
    {
        for (size_t i{}; i < field_matr.size() - 1; ++i)
        {
            for (size_t j{ i + 1 }; j < field_matr.size(); ++j)
            {
                std::swap
                (
                    field_matr[i][j],
                    field_matr[j][i]
                );
            }
        }
    }

    static
        void    reverse_strings(T_field_matr& field_matr)
    {
        std::reverse
        (
            field_matr.begin(),
            field_matr.end()
        );
    }

    void    calc_children_and_set_their_results_for_computer
    (
        T_position                  const& parent_position,
        T_results_set_for_computer& children_results_for_computer
    )
    {
        auto    parent_field_str = parent_position.field_str_;

        for (size_t i{}; i < parent_field_str.size(); ++i)
        {
            if (
                parent_field_str[i]
                != SPACE_SYMB
                )
            {
                continue;
            }

            auto    child_cur_field_str(parent_field_str);

            child_cur_field_str[i] = parent_position.made_computer_
                ? player_symb_
                : computer_symb_;

            children_results_for_computer.insert
            (
                get_position_for_field_str(child_cur_field_str).res_for_computer_
            );
        }
    }
};
int     main()
{
    T_xs_and_os     xs_and_os;

    for (;;)
    {
        xs_and_os.input_player_symb();
        xs_and_os.play();
    }
}