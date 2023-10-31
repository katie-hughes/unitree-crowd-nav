#include "brnelib/brne.hpp"

namespace brne
{
  BRNE::BRNE(double kernel_a1, double kernel_a2,
             double cost_a1, double cost_a2, double cost_a3,
             double dt, int n_steps, int n_samples,
             double max_ang_vel, double max_lin_vel,
             double y_min, double y_max):
    kernel_a1{kernel_a1},
    kernel_a2{kernel_a2},
    cost_a1{cost_a1},
    cost_a2{cost_a2},
    cost_a3{cost_a3},
    dt{dt},
    n_steps{n_steps},
    n_samples{n_samples},
    max_ang_vel{max_ang_vel},
    max_lin_vel{max_lin_vel},
    y_min{y_min},
    y_max{y_max}
    {}
  void BRNE::print_params(){
    std::cout << "kernel a1:\t" << kernel_a1 << std::endl;
    std::cout << "kernel a2:\t" << kernel_a2 << std::endl;
    std::cout << "cost a1:\t" << cost_a1 << std::endl;
    std::cout << "cost a2:\t" << cost_a2 << std::endl;
    std::cout << "cost a3:\t" << cost_a3 << std::endl;
    std::cout << "dt:\t\t" << dt << std::endl;
    std::cout << "n steps:\t" << n_steps << std::endl;
    std::cout << "n samples:\t" << n_samples << std::endl;
    std::cout << "max ang vel:\t" << max_ang_vel << std::endl;
    std::cout << "max lin vel:\t" << dt << std::endl;
    std::cout << "y min:\t\t" << dt << std::endl;
    std::cout << "y max:\t\t" << dt << std::endl;
  }

  arma::mat BRNE::compute_kernel_mat(arma::vec t1, arma::vec t2){
    arma::mat res(t1.size(), t2.size(), arma::fill::zeros);
    for (auto i=0; i<t1.size(); i++){
      for (auto j=0; j<t2.size(); j++){
        res.at(i,j) = kernel_a2 * exp(-kernel_a1 * pow((t1.at(i) - t2.at(j)),2));
      }
    }
    return res;
  }

  void BRNE::compute_Lmat(){
    // arma::vec tlist(n_steps, arma::fill::zeros);
    // for (auto i=0; i<n_steps; i++){
    //   tlist.at(i) = i*dt;
    // }
    arma::vec tlist = arma::linspace<arma::vec>(0, (n_steps-1)*dt, n_steps);
    std::cout << "Tlist\n" << tlist << std::endl;
    arma::vec train_ts(1, arma::fill::value(tlist.at(0)));
    std::cout << "train ts\n" << train_ts << std::endl;
    arma::vec train_noise(1, arma::fill::value(1e-3));
    std::cout << "Train noise\n" << train_noise << std::endl;

    auto cm_11 = compute_kernel_mat(train_ts, train_ts);
    cm_11 += train_noise.diag();
    auto cm_12 = compute_kernel_mat(tlist, train_ts);
    auto cm_22 = compute_kernel_mat(tlist, tlist);
    cov_mat = cm_22 - cm_12 * cm_11.i() * cm_12.t();
    std::cout << "Cov mat\n" << cov_mat << std::endl;
    cov_Lmat = arma::chol(cov_mat, "lower");
    std::cout << "Cov Lmat\n" << cov_Lmat << std::endl;
  }

  arma::mat BRNE::mvn_sample_normal(){
    // TODO should do error checking to make sure Lmat is set
    arma::mat res(n_steps, n_samples, arma::fill::randn);
    std::cout << "Random sample\n" << res << std::endl;
    return (cov_Lmat * res).t();
  }
}