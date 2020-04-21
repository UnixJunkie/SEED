library('tidyverse')

df <- tibble(fun_name = c('ElecFrag', 'SqDisFrRe_ps_vdW', 'others'), ncalls = c(1000, 5000, 2), perc_time = c(60.58,13.60, 100-60.58-13.60))
tot_exec_time <- 45.0 # seconds
nfrag <- 300

tmp <- df %>% 
  mutate(fun_time = tot_exec_time*perc_time/100.0) %>% 
  mutate(fun_time_per_call = fun_time / ncalls) %>% 
  mutate(new_ncalls = ncalls/2*nfrag) %>% 
  mutate(new_fun_time = new_ncalls * fun_time_per_call)

#tmp$new_fun_time/sum(tmp$new_fun_time)
  
