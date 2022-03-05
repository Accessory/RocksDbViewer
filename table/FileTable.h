#pragma once

#include <FL/Fl_Table_Row.H>
#include <FL/fl_draw.H>
#include <vector>
#include <FlowUtils/FlowLog.h>
#include <algorithm>
#include <FlowCurl/PremiumizeMe_Structs.h>
#include "TableModel.h"
#include "../Style.h"

#ifdef max
#undef max
#endif

class FileTable : public Fl_Table_Row {
public:
    FileTable(int X, int Y, int W, int H, const char *L = 0) : Fl_Table_Row(X, Y, W, H, L) {
        cols(3);
        col_header(1);              // enable column headers (along top)
        col_width_all(80);          // default width of columns
        col_resize(1);              // enable column resizing
        col_width(0, 280);

        row_height_all(20);         // default height of rows
        row_resize(0);              // disable row resizing
        end();
    }

    void setData(std::vector<TableModel> data) {
        this->data = std::move(data);
        rows(this->data.size());
    }

    std::vector<TableModel> getSelectedData() {
//        int row_top, col_left, row_bot, col_right;
//        get_selection(row_top, col_left, row_bot, col_right);
//
//        std::vector<TableModel> rtn;
//        if (row_top == -1)
//            return rtn;
//        for (int i = row_top; i <= row_bot; ++i) {
//            rtn.emplace_back(data.at(i));
//        }
//
//        return rtn;
        std::vector<TableModel> rtn;
        for (size_t i = 0; i < data.size(); ++i) {
            if (row_selected(i)) {
                rtn.emplace_back(data.at(i));
            }
        }
        return rtn;
    }


private:

    std::vector<TableModel> data;

    // Draw the row/col headings
    //    Make this a dark thin upbox with the text inside.
    //
    void DrawHeader(const char *s, int X, int Y, int W, int H) {
        fl_push_clip(X, Y, W, H);
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BORDER_BOX);
        fl_color(TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b);
        fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
        fl_pop_clip();
    }

    // Draw the cell data
    //    Dark gray text on white background with subtle border
    //
    void DrawData(const char *s, int R, int C, int X, int Y, int W, int H) {
        fl_push_clip(X, Y, W, H);
        // Draw cell bg
        if (row_selected(R)) {
            fl_color(SELECTED_BACKGROUND.r, SELECTED_BACKGROUND.g, SELECTED_BACKGROUND.b);
        } else {
            fl_color(FL_BACKGROUND2_COLOR);
        }
        fl_rectf(X, Y, W, H);
        // Draw cell data
        fl_color(TEXT_COLOR.r, TEXT_COLOR.g, TEXT_COLOR.b);
        fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
        // Draw box border
        fl_color(FL_BACKGROUND_COLOR);
        fl_rect(X, Y, W, H);
        fl_pop_clip();
    }

    // Draw the cell data
    //    Dark gray text on white background with subtle border
    //
    void DrawStatus(double p, const char *s, int R, int C, int X, int Y, int W, int H) {
        if (p == 0) {
            DrawData(s, R, C, X, Y, W, H);
            return;
        }
        fl_push_clip(X, Y, W, H);
        // Draw cell bg

        fl_color(fl_rgb_color(51, 122, 183));
        int W1 = W * p;
        int W2 = W - W1;
        fl_rectf(X, Y, W1, H);
        fl_color(fl_rgb_color(233, 236, 239));
        fl_rectf(X + W1, Y, W2, H);
        // Draw cell data
        fl_color(FL_GRAY0);
        fl_draw(s, X, Y, W, H, FL_ALIGN_CENTER);
        // Draw box border
        fl_color(color());
        fl_rect(X, Y, W, H);
        fl_pop_clip();
    }


    // Handle drawing table's cells
    //     Fl_Table calls this function to draw each visible cell in the table.
    //     It's up to us to use FLTK's drawing functions to draw the cells the way we want.
    //
    void draw_cell(TableContext context, int ROW = 0, int COL = 0, int X = 0, int Y = 0, int W = 0, int H = 0) {

        switch (context) {
            case CONTEXT_RC_RESIZE: {
                const auto fullSize = sizes()[1];
                auto lastColumnSize = std::max(fullSize - col_width(0) - col_width(1) - 22, 40);
                if (data.size() > 20) {
                    lastColumnSize = std::max(0, lastColumnSize - 16);
                }
                col_width(2, lastColumnSize);
                return;
            }
            case CONTEXT_STARTPAGE: {                // before page is drawn..
                fl_font(labelfont(), labelsize());              // set the font for our drawing operations
                row_header(false);
                return;
            }
            case CONTEXT_COL_HEADER: {               // Draw column headers
                switch (COL) {
                    case 0: {
                        DrawHeader("id", X, Y, W, H);
                        break;
                    }
                    case 1: {
                        DrawHeader("status", X, Y, W, H);
                        break;
                    }
                    case 2: {
                        DrawHeader("name", X, Y, W, H);
                        break;
                    }
                }
                return;
            }
            case CONTEXT_ROW_HEADER:                  // Draw row headers
//                sprintf(s, "%03d:", ROW);                 // "001:", "002:", etc
//                DrawHeader(s, X, Y, W, H);
                return;
            case CONTEXT_CELL: {                     // Draw data in cells
                switch (COL) {
                    case 0: {
                        DrawData(data.at(ROW).id.c_str(), ROW, COL, X, Y, W, H);
                        break;
                    }
                    case 1: {
                        DrawStatus(data.at(ROW).progress, data.at(ROW).status.c_str(), ROW, COL, X, Y, W, H);
                        break;
                    }
                    case 2: {
                        DrawData(data.at(ROW).name.c_str(), ROW, COL, X, Y, W, H);
                        break;
                    }
                }
                return;
            }
            default:
                return;
        }
    }
};


