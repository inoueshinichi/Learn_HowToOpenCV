/**
 * @file opticalflow_lucaskanade.cpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief オプティカルフロー Lucas-Kanade法 注目点の周辺3x3の領域に輝度勾配が同じという制約条件をつける
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <test_utils.hpp>
#include <opencv2/opencv.hpp>

// スルー画像の取り込み処理
CV_DEFINE_PROCESSFRAME

/**
 * @brief オプティカルフロー
 * @note 時間的に連続する2画像間の差を元に求まる2次元ベクトル場
 * @note 下記, 前提
 * 微小な時間Δtの間に時刻tにおける座標(x,y)にある対象物体の画素値I(x,y,t)
 * と, 時刻t+Δtにおける座標(x+Δx,y+Δy)に移動した対象物体の画素値I(x+Δx,y+Δy,t+Δt)
 * が等しい.
 *
 * 仮定(1)
 * I(x,y,t) = I(x+Δx,y+Δy,t+Δt)
 *
 * ※右辺をテイラー展開(2次以上の項を除去)
 * ※ 変数ベクトル(x,y,t)でそれぞれ偏微分する
 * ※ 偏微分 e.g. 変数xで微分. d{I(x,y,t)}/dx = I_x(x,y,t)*I(x,y,t)
 *
 * I(x,y,t) =: I(x,y,t) + {dI(x,y,t)/dx}*Δx + {dI(x,y,t)/dy}*Δy + {dI(x,y,t)/dt}*Δt
 *          =: I(x,y,t) + I_x(x,y,t)*I(x,y,t)*Δx + I_y(x,y,t)*I(x,y,t)*Δy + I_t(x,y,t)*I(x,y,t)*Δt　・・・(A)
 *
 * I_x(x,y,t) : 座標(x,y)のx方向の輝度勾配
 * I_y(x,y,t) : 座標(x,y)のy方向の輝度勾配
 * I_t(x,y,t) : 座標(x,y)の輝度のフレーム方向の勾配(=フレーム間差分)
 *
 * 両辺(A)のI(x,y,t)を相殺.
 *
 * I_x(x,y,t)*I(x,y,t)*Δx + I_y(x,y,t)*I(x,y,t)*Δy + I_t(x,y,t)*I(x,y,t)*Δt = 0 ・・・(B)
 *
 * 両辺(B)をI(x,y,t)*Δtで除算
 *
 * I_x(x,y,t)*(Δx/Δt) + I_y(x,y,t)*(Δy/Δt) + I_t(x,y,t) = 0 ・・・(C)
 *
 * (C)・・・オプティカルフローの拘束条件
 *
 * 時刻tの座標(x,y)のオプティカルフローを
 * ・u=(Δx/Δt)
 * ・v=(Δy/Δt)
 * とすると,
 *
 * I_x(x,y,t)*u + I_y(x,y,t)*v = -I_t(x,y,t) ・・・(D)
 *
 * 式(D)の1つだけだと2個の未知数(u,v)を解くことはできないので, 拘束条件を追加する.
 *
 * 仮定(2) Lucas-Kanade法
 * ある画素の近傍画素も同じオプティカルフローを示すという仮定(拘束条件)を追加する.
 * 具体的には, 注目画素(x,y)の周囲3x3の領域に下記の9つの条件が成り立つと仮定する.
 *
 * ◯・・・注目画素
 * ◯,□・・・3x3領域
 * | □_1 | □_2 | □_3 |
 * | □_4 | ◯_5 | □_6 |
 * | □_7 | □_8 | □_9 |
 *
 * □_1 : I_x(x-1,y-1,t) * u + I_y(x-1,y-1,t) * v = -I_t(x-1,y-1,t)
 * □_2 : I_x(x,y-1,t)   * u + I_y(x,y-1,t)   * v = -I_t(x_t,y-1,t)
 * □_3 : I_x(x+1,y-1,t) * u + I_y(x+1,y-1,t) * v = -I_t(x+1,y-1,t)
 * □_4 : I_x(x-1,y,t)   * u + I_y(x-1,y,t)   * v = -I_t(x-1,y,t)
 * ◯_5 : I_x(x,y,t)     * u + I_y(x,y,t)     * v = -I_t(x,y,t)
 * □_6 : I_x(x+1,y,t)   * u + I_y(x+1,y,t)   * v = -I_t(x+1,y,t)
 * □_7 : I_x(x-1,y-1,t) * u + I_y(x-1,y-1,t) * v = -I_t(x-1,y-1,t)
 * □_8 : I_x(x,y-1,t)   * u + I_y(x,y-1,t)   * v = -I_t(x,y-1,t)
 * □_9 : I_x(x+1,y+1,t) * u + I_y(x+1,y+1,t) * v = -I_t(x+1,y+1,t)
 *
 * 1次連立方程式にして, 最小二乗法で解く
 * Ax=b
 * min J = ||Ax - b||^2
 * ∇J=0となるx=[u v]を求める.
 *
 * A = [
 * [ I_x(x-1,y-1,t) I_y(x-1,y-1,t) ]
 * [ I_x(x,y-1,t)   I_y(x,y-1,t)   ]
 * [ I_x(x+1,y-1,t) I_y(x+1,y-1,t) ]
 * [ I_x(x-1,y,t)   I_y(x-1,y,t)   ]
 * [ I_x(x,y,t)     I_y(x,y,t)     ]
 * [ I_x(x+1,y,t)   I_y(x+1,y,t)   ]
 * [ I_x(x-1,y-1,t) I_y(x-1,y-1,t) ]
 * [ I_x(x,y-1,t)   I_y(x,y-1,t)   ]
 * [ I_x(x+1,y+1,t) I_y(x+1,y+1,t) ]]
 *
 * x = [ u v ]
 *
 * b = [
 * [ -I_t(x-1,y-1,t) ]
 * [ -I_t(x_t,y-1,t) ]
 * [ -I_t(x+1,y-1,t) ]
 * [ -I_t(x-1,y,t)   ]
 * [ -I_t(x,y,t)     ]
 * [ -I_t(x+1,y,t)   ]
 * [ -I_t(x-1,y-1,t) ]
 * [ -I_t(x,y-1,t)   ]
 * [-I_t(x+1,y+1,t)  ]]
 *
 * ∇J = [ dJ/du dJ/dv ]
 *
 * J = ||Ax - b||^2を展開して整理すると
 *   = (A^t*A*x, x) - 2 * (A^t*b, x) - ||b||^2
 *
 * ∇J = A^t*A*x - A^t*b
 * A^t*A*x - A^t*b = 0
 *
 * 最小二乗解
 * x = (A^t*A)^-1 * (A^t*b)
 *
 * U = A^t*A : (2,2)
 * V = A^t*b : (2,1)
 *
 * U[0,0] = sum_{i=-1}^{1}sum{j=-1}^{1} I_x(x+i,y+i,t)^2
 * U[0,1] = sum_{i=-1}^{1}sum{j=-1}^{1} I_x(x+i,y+i,t)*I_y(x+i,y+i,t)
 * U[1,0] = sum_{i=-1}^{1}sum{j=-1}^{1} I_x(x+i,y+i,t)*I_y(x+i,y+i,t)
 * U[1,1] = sum_{i=-1}^{1}sum{j=-1}^{1} I_y(x+i,y+i,t)^2
 *
 * V[0,0] = -sum_{i=-1}^{1}sum{j=-1}^{1} I_x(x+i,y+i,t)*I_t(x+i,y+i,t)
 * V[1,0] = -sum_{i=-1}^{1}sum{j=-1}^{1} I_y(x+i,y+i,t)*I_t(x+i,y+i,t)
 */

int main(int argc, char **argv)
{
    try
    {
        /**
         * @brief OpticalFlow
         * 1) Lucas-Kanade法
         * 2) Gunnar-Farnebackアルゴリズム
         */

        // 画像内からオプティカルフローを計算するために使う画像メモリ
        cv::Mat img_src;
        cv::Mat img_gray_prev;
        cv::Mat img_gray;
        std::vector<cv::Point2f> flow_points_current;
        std::vector<cv::Point2f> flow_points_last;
        cv::Mat img_flow_status; // フローを取得: 0, 失敗: 未定義
        cv::Mat img_flow_error;  // 移動前の特長点の周辺領域と, 移動後の特徴点の周辺領域との画素値の差を周辺領域の画素数で割った値
        cv::Mat img_flow_dst; // オプティカルフローの結果

        // フローを取得するピクセルの間隔
        const int flow_stride_w = 10;
        const int flow_stride_h = 10;

        // 表示用
        cv::Mat img_bin_rgb;
        cv::Mat img_raw_resized;
        cv::Mat img_bin_resized;

        auto optical_flow = [&](const cv::Mat &curFrame,
                                const cv::Mat &lastFrame) -> void
        {
            // 現フレーム画像
            img_src = curFrame;

            // グレースケールに変換
            cv::cvtColor(lastFrame, img_gray_prev, cv::COLOR_BGR2GRAY); // 1フレーム前
            cv::cvtColor(curFrame, img_gray, cv::COLOR_BGR2GRAY); // 現フレーム

            /* オプティカルフロー */

            // フローを取得したい座標(x,y) 10ピクセル間隔に設定
            flow_points_last.clear();
            flow_points_current.clear();
            for (int y = 0; y < img_gray.rows; y += flow_stride_h)
            {
                for (int x = 0; x < img_gray.cols; x += flow_stride_w)
                {
                    flow_points_last.emplace_back(x,y);
                }
            }

            // フローを計算
            cv::calcOpticalFlowPyrLK(
                /* prevImg= */ img_gray_prev,       // cv::InputArray nextImg
                /* nextImg= */ img_gray,            // cv::InputArray nextImg
                /* prevPts= */ flow_points_last,    // cv::InputArray prevPts (cv:Point2f array)
                /* nextPts= */ flow_points_current, // cv::InputOutputArray nextPts (cv:Point2f array)
                /* status= */ img_flow_status,      // cv::OutputArray status
                /* err= */ img_flow_error,          // cv::OutputArray err
                /* winSize= */ cv::Size(21, 21),    // cv::Size winSize=cv::Size(21,21),
                /* maxLevel= */3                    // int maxLevel=3,
                /* cv::TermCriteria criteria =
                cv::TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 30, (0.01000000000000000021)) */
                /* int flags = 0 */
                /* double minEigThreshold = (0.0001000000000000000048)) */
            );

            // フローの描画
            img_src.copyTo(img_flow_dst); // 現フレーム画像をコピー
            for (int i = 0; i < flow_points_current.size(); ++i)
            {
                cv::arrowedLine(
                    /* img= */ img_flow_dst,
                    /* pt1= */ flow_points_last.at(i),
                    /* pt2= */ flow_points_current.at(i),
                    /* color= */ cv::Scalar(0, 0, 255),  // const cv::Scalar&
                    /* thickness= */ 1,                  // int thickness
                    /* line_type= */ 8,                  // int line_type
                    /* shift= */ 0,                      // int shift
                    /* tipLength= */ 1.0                 // double tipLength
                );
            }

            // 表示用画像
            int width = curFrame.cols;
            int height = curFrame.rows;
            int half_width = (int)(width / 2);
            int half_height = (int)(height / 2);
            cv::resize(img_src, img_raw_resized, cv::Size(half_width, half_height));
            cv::resize(img_flow_dst, img_bin_resized, cv::Size(half_width, half_height));
            cv::Mat img_show = cv::Mat::zeros(cv::Size(width, height), curFrame.type());

            // 部分領域に書き込む
            img_raw_resized.copyTo(img_show(cv::Rect(0, 0, half_width, half_height)));                    // current(左上)
            img_bin_resized.copyTo(img_show(cv::Rect(half_width, half_height, half_width, half_height))); // last(右下)

            CV_IMSHOW(img_show)
            // CV_IMSHOW(curFrame)
            // CV_IMSHOW(lastFrame)
        };

        ProcessFrame(0, optical_flow);
    }
    catch (const char *str)
    {
        std::cerr << str << std::endl;
    }
    return 0;
}