#pragma once

#include <limits>
#include <vector>

namespace soro {

using probability_t = float;

template <typename T>
struct prob_dist_iter {
  using container_type = typename T::container_type;
  using base_iterator_t = typename container_type::const_iterator;
  using primary_t = typename T::primary_t;
  using value_type = typename container_type::value_type;

  explicit prob_dist_iter(T const& dist, base_iterator_t it)
      : t_{dist.first_}, it_(std::move(it)) {}

  prob_dist_iter& operator++() {
    ++it_;
    ++t_;
    return *this;
  }

  prob_dist_iter& operator--() {
    --it_;
    --t_;
    return *this;
  }

  prob_dist_iter& operator+=(int off) {
    it_ += off;
    t_ += off;
    return *this;
  }

  std::pair<primary_t, value_type> operator*() const { return {t_, *it_}; }

  bool operator==(prob_dist_iter const& o) const { return it_ == o.it_; }
  bool operator!=(prob_dist_iter const& o) const { return it_ != o.it_; }

  int operator-(prob_dist_iter const& o) const {
    return std::distance(it_, o.it_);
  }

  typename T::primary_t t_;
  base_iterator_t it_;
};

template <typename T>
prob_dist_iter(T const&, typename T::container_type::const_iterator)
    -> prob_dist_iter<T>;

template <typename Granularity, typename... Ts>
struct dpd {};

template <typename Granularity, typename T>
struct dpd<Granularity, T> {
  using primary_t = T;
  using container_type = std::vector<probability_t>;

  dpd() = default;
  explicit dpd(T t) : first_{std::move(t)}, dpd_({probability_t{1.0}}) {}
  auto begin() const { return prob_dist_iter{*this, std::cbegin(dpd_)}; }
  auto end() const { return prob_dist_iter{*this, std::cend(dpd_)}; }
  bool empty() const { return dpd_.empty(); }

  probability_t& operator[](primary_t const i) { return dpd_[to_idx(i)]; }

  static constexpr primary_t to_idx(primary_t const i) {
    constexpr auto const granularity = Granularity{}.template get<0>();
    return i / granularity;
  }

  T first_{std::numeric_limits<T>::max()};
  std::vector<probability_t> dpd_;
};

template <typename Granularity, typename T, typename... Ts>
struct dpd<Granularity, T, Ts...> {
  using primary_t = T;
  using container_type = std::vector<dpd<Granularity, Ts...>>;

  dpd() = default;

  explicit dpd(T head, Ts... tail) : first_{std::move(head)} {
    dpd_.emplace_back(tail...);
  }
  auto begin() const { return prob_dist_iter{*this, std::cbegin(dpd_)}; }
  auto end() const { return prob_dist_iter{*this, std::cend(dpd_)}; }
  bool empty() const { return dpd_.empty(); }

  dpd<Granularity, Ts...>& operator[](primary_t const i) {
    return dpd_[to_idx(i)];
  }

  static constexpr primary_t to_idx(primary_t const i) {
    constexpr auto const granularity = Granularity{}.template get<0>();
    return i / granularity;
  }

  T first_{std::numeric_limits<primary_t>::max()};
  std::vector<dpd<Granularity, Ts...>> dpd_;
};

}  // namespace soro
