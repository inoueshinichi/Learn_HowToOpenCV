/**
 * @file particle_filter.hpp
 * @author Shinichi Inoue (inoue.shinichi.1800@gmail.com)
 * @brief パーティクルフィルタの独自実装
 * @version 0.1
 * @date 2023-08-14
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#include <vector>
#include <tuple>
#include <cmath>
#include <random>
#include <algorithm>
#include <type_traits> // std::enable_if_t

namespace is
{
    // Base Particle
    class Particle
    {
    public:
        Particle(double likelihood, double weight, bool keep)
            : _likelihood(likelihood), _weight(weight), _keep(keep)
        {
        }

        virtual ~Particle() = default;
        Particle(const Particle &other) = default;
        Particle &operator=(const Particle &right) = default;
        Particle(Particle &&other) = default;
        Particle &operator=(Particle &&right) = default;

        double getLikelihood() const
        {
            return _likelihood;
        }

        void setLikelihood(double value)
        {
            _likelihood = value;
        }

        bool getKeep() const { return _keep; }

        void setKeep(bool flag) { _keep = flag; }

        double getWeight() const { return _weight; }

        void setWeight(double value) { _weight = value; }

        // std::sort用
        bool operator<(const Particle &right) const
        {
            return this->_likelihood < right._likelihood;
        }

    protected:
        // 尤度
        double _likelihood;
        // 重み
        double _weight;
        // 残存フラグ
        bool _keep;
    };

    // // 3次元点群 パーティクル
    // class PointCloud3dParticle final : public Particle
    // {
    // public:
    //     PointCloud3dParticle(float pos_x, float pos_y, float pos_z,
    //                          float vel_x, float vel_y, float vel_z,
    //                          double likelihood = 1.0, double weight = 0.0, bool keep = false)
    //         : Particle(likelihood, weight, keep), _px(pos_x), _py(pos_y), _pz(pos_z), _vx(vel_x), _vy(vel_y), _vz(vel_z)
    //     {
    //     }

    // public:
    //     // position
    //     float _px;
    //     float _py;
    //     float _pz;

    //     // velocity
    //     float _vx;
    //     float _vy;
    //     float _vz;
    // };

    // 2次元点群 パーティクル
    class PointCloud2dParticle final : public Particle
    {
    public:
        PointCloud2dParticle(float pos_x, float pos_y,
                             float vel_x, float vel_y,
                             double likelihood = 1.0, double weight = 0.0, bool keep = false)
            : Particle(likelihood, weight, keep), _px(pos_x), _py(pos_y), _vx(vel_x), _vy(vel_y)
        {}

        ~PointCloud2dParticle() {}
        PointCloud2dParticle(const PointCloud2dParticle &) = default;
        PointCloud2dParticle &operator=(const PointCloud2dParticle &) = default;
        PointCloud2dParticle(PointCloud2dParticle &&) = default;
        PointCloud2dParticle &operator=(PointCloud2dParticle &&) = default;

    public:
        // position
        float _px;
        float _py;

        // velocity
        float _vx;
        float _vy;
    };

    ////////////////////////

    // パーティクルフィルタ
    template <typename PARTICLE, typename = std::enable_if_t<std::is_base_of_v<is::Particle, PARTICLE>>>
    class ParticleFilter
    {
    public:
        ParticleFilter(unsigned int num_particles,
                       double likelihood_threshold,
                       int remaining_percentage,
                       unsigned int seed, bool deterministic = true)
            : _num_particles(num_particles)
            , _likelihood_threshold(likelihood_threshold)
            , _remaining_persentage(remaining_percentage < 100 ? remaining_percentage : 100)
            , _seed(seed)
            , _deterministic(deterministic)
            , _initialized(false)
            , _engine()
            , _sum_likelihood(0.0)
        {
            // 非決定論シード数
            if (!_deterministic)
            {
                std::random_device rd;
                std::random_device::result_type seed_rd = rd(); // unsigned int
                _seed = seed_rd;                                // 予測不可能な32bit整数を生成
            }
            _engine.seed(_seed); // 乱数エンジンのシードを変更
        }

        ~ParticleFilter() {}
        ParticleFilter(const ParticleFilter &) = delete;
        ParticleFilter &operator=(const ParticleFilter &) = delete;
        ParticleFilter(ParticleFilter &&) = default;
        ParticleFilter &operator=(ParticleFilter &&) = default;

        void setLikelihoodThreshold(double threshold)
        {
            _likelihood_threshold = threshold;
        }

        void initialize(const std::function<PARTICLE(std::mt19937 &engine)> &initParticle)
        {
            // パーティクルの初期化
            for (unsigned int i = 0; i < _num_particles; i++)
            {
                PARTICLE p = initParticle(_engine);
                _particles.push_back(p);
            }
            _initialized = true;
        }

        void update(const std::function<void(PARTICLE &)> &updateLikelihood)
        {
            if (!_initialized)
                throw std::runtime_error("No initialized.");

            // パーティクルの尤度更新
            for (unsigned int i = 0; i < _particles.size(); i++)
            {
                updateLikelihood(_particles.at(i));
            }

            // 尤度に従って, パーティクルを昇順ソート
            std::sort(_particles.begin(), _particles.end());

            // 刈り取り処理 (Prune)
            // 尤度の高いパーティクルを残し, 尤度の低いパーティクルを消す
            int keep_particles = int(_particles.size() * (_remaining_persentage / 100.0)); // 残存パーティクル数
            for (unsigned int i = 0; i < _particles.size(); i++)
            {
                if (_particles[i].getLikelihood() > _likelihood_threshold || // 尤度閾値より高い
                    i > (_particles.size() - keep_particles))                // 残存パーティクル数に該当
                {
                    _particles[i].setKeep(true);
                }
                else
                {
                    _particles[i].setKeep(false);
                }
            }

            auto iter = _particles.begin();
            while (iter != _particles.end())
            {
                if ((*iter).getKeep())
                {
                    ++iter;
                }
                else
                {
                    iter = _particles.erase(iter); // 現在のiter先を削除して次のiterを取得.
                }
            }

            // 尤度の高い残存パーティクル数の尤度の合計
            _sum_likelihood = 0.0; // 初期化
            for (const auto &p : _particles)
            {
                _sum_likelihood += p.getLikelihood();
            }

            // 尤度に従った重みの正規化
            for (auto &p : _particles)
            {
                p.setWeight(p.getLikelihood() / _sum_likelihood);
            }
        }

        void resampling(const std::function<PARTICLE(PARTICLE &, std::mt19937 &)> &genNewParticle)
        {
            if (!_initialized)
                throw std::runtime_error("No initialized.");

            // 初期化
            _new_particles.clear();

            for (unsigned int i = 0; i < _particles.size(); i++)
            {
                // 各パーティクル毎に次刻の想定される状態数をシミュレート
                unsigned int new_num_particles = _particles[i].getWeight() * (_num_particles - _particles.size());

                for (unsigned int j = 0; j < new_num_particles; j++)
                {
                    PARTICLE new_p = genNewParticle(_particles[i], _engine);
                    _new_particles.push_back(new_p);
                }
            }

            // パーティクルの更新
            std::copy(_new_particles.begin(), _new_particles.end(), std::back_inserter(_particles)); // push_back to _particles
        }

        void predict(const std::function<void(PARTICLE &)> &updateParticle)
        {
            if (!_initialized)
                throw std::runtime_error("No initialized.");

            // パーティクルの予測
            for (unsigned int i = 0; i < _particles.size(); i++)
            {
                updateParticle(_particles.at(i));
            }
        }

        void setSeed(unsigned int seed)
        {
            _seed = seed;
            _engine.seed(_seed); // 乱数エンジンの内部状態が変更される
        }

        std::vector<PARTICLE> &getParticles() { return _particles; }
        std::vector<PARTICLE> &getNewParticles() { return _new_particles; }

    private:
        std::vector<PARTICLE> _particles;
        std::vector<PARTICLE> _new_particles;
        unsigned int _num_particles;  // 初期パーティクル数
        double _likelihood_threshold; // 残存パーティクルの尤度閾値
        int _remaining_persentage;    // 残存パーティクルの割合
        double _sum_likelihood;
        bool _initialized;

        bool _deterministic;
        unsigned int _seed;
        std::mt19937 _engine; // メルセンヌ・ツイスター
    };
}