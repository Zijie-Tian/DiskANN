#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <cassert>
#include <fstream>

#define HZ hz
#define S_VALUE(m, n, p) (((double) ((n) - (m))) / (p) *HZ)
#define DISKSTATS_FILE "/proc/diskstats"
#define STAT_FILE "/proc/stat"

namespace diskann {
  unsigned int hz;

  struct dstats {
    int           major;
    int           minor;
    char          devname[64];
    unsigned long r_completed;    /* reads completed successfully */
    unsigned long r_merged;       /* reads merged */
    unsigned long r_sectors;      /* sectors read */
    unsigned long r_spent;        /* time spent reading (ms) */
    unsigned long w_completed;    /* writes completed */
    unsigned long w_merged;       /* writes merged */
    unsigned long w_sectors;      /* sectors written */
    unsigned long w_spent;        /* time spent writing (ms) */
    unsigned long io_in_progress; /* I/Os currently in progress */
    unsigned long t_spent;        /* time spent doing I/Os (ms) */
    unsigned long t_weighted;     /* weighted time spent doing I/Os (ms) */

    unsigned long long uptime;
  };

  struct ext_dstats {
    double util;
    double await;
    double arqsz;
  };

  struct iostats {
    char   devname[64];
    double rrqm_s;
    double wrqm_s;
    double r_s;
    double w_s;
    double rmb_s;
    double wmb_s;
    double avgrq_sz;
    double avgqu_sz;
    double await;
    double r_await;
    double w_await;
    double until;
  };

  inline void get_HZ(void) {
    long ticks;
    if ((ticks = sysconf(_SC_CLK_TCK)) == -1) {
      perror("sysconf");
    }
    hz = (unsigned int) ticks;
  }

  inline void read_uptime(unsigned long long *uptime) {
    FILE         *fp;
    char          line[128];
    unsigned long up_sec, up_cent;

    if ((fp = fopen("/proc/uptime", "r")) == NULL)
      return;

    if (fgets(line, sizeof(line), fp) == NULL) {
      fclose(fp);
      return;
    }

    sscanf(line, "%lu.%lu", &up_sec, &up_cent);
    *uptime = (unsigned long long) up_sec * HZ +
              (unsigned long long) up_cent * HZ / 100;
    fclose(fp);
  }

  inline void replace_dstats(struct dstats *curr[], struct dstats *prev[],
                             int n_dev) {
    int i;
    for (i = 0; i < n_dev; i++) {
      prev[i]->r_completed = curr[i]->r_completed;
      prev[i]->r_merged = curr[i]->r_merged;
      prev[i]->r_sectors = curr[i]->r_sectors;
      prev[i]->r_spent = curr[i]->r_spent;
      prev[i]->w_completed = curr[i]->w_completed;
      prev[i]->w_merged = curr[i]->w_merged;
      prev[i]->w_sectors = curr[i]->w_sectors;
      prev[i]->w_spent = curr[i]->w_spent;
      prev[i]->io_in_progress = curr[i]->io_in_progress;
      prev[i]->t_spent = curr[i]->t_spent;
      prev[i]->t_weighted = curr[i]->t_weighted;

      prev[i]->uptime = curr[i]->uptime;
    }
  }

  inline unsigned long long get_interval(unsigned long long prev_uptime,
                                         unsigned long long curr_uptime) {
    unsigned long long itv;

    /* first run prev_uptime=0 so displaying stats since system startup */
    itv = curr_uptime - prev_uptime;

    if (!itv) { /* Paranoia checking */
      itv = 1;
    }
    return itv;
  }

  inline int get_curr_n_dev() {
    char  ch;
    int   curr_n_dev = 0;
    FILE *diskstats_fp;
    diskstats_fp =
        fopen(DISKSTATS_FILE, "r"); /* todo: sanity check if open fails - писать
                                       ошибку что не могу прочитать */
    while (!feof(diskstats_fp)) {
      ch = fgetc(diskstats_fp);
      if (ch == '\n')
        curr_n_dev++;
    }
    fclose(diskstats_fp);

    return curr_n_dev;
  }

  inline void get_curr_bw(struct dstats     *curr_dstats[],
                          struct dstats     *prev_dstats[],
                          struct ext_dstats *xds[], double *r_await,
                          double *w_await, struct iostats *iostats[],
                          int n_dev) {
    FILE *diskstats_fp;
    diskstats_fp =
        fopen(DISKSTATS_FILE, "r"); /* todo: sanity check if open fails - писать
                                       ошибку что не могу прочитать */

    // diskstats_fp =
    //     fopen(DISKSTATS_FILE, "r"); /* todo: sanity check if open fails*/

    if (diskstats_fp == NULL) {
      printf("Error: %s\n", strerror(errno));
      exit(1);
    }

    int           major, minor;
    char          devname[64];
    char          line[128];
    unsigned long r_completed, r_merged, r_sectors, r_spent, w_completed,
        w_merged, w_sectors, w_spent, io_in_progress, t_spent, t_weighted;

    unsigned long long curr_uptime;
    read_uptime(&(curr_uptime));

    int i = 0;
    while (fgets(line, sizeof(line), diskstats_fp) != NULL) {
      sscanf(line, "%i %i %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
             &major, &minor, devname, &r_completed, &r_merged, &r_sectors,
             &r_spent, &w_completed, &w_merged, &w_sectors, &w_spent,
             &io_in_progress, &t_spent, &t_weighted);

      // printf("%s\t %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",
      //        devname, r_completed, r_merged, r_sectors, r_spent, w_completed,
      //        w_merged, w_sectors, w_spent, io_in_progress, t_spent,
      //        t_weighted, major, minor);
      curr_dstats[i]->major = major;
      curr_dstats[i]->minor = minor;
      strcpy(curr_dstats[i]->devname, devname);
      curr_dstats[i]->r_completed = r_completed;
      curr_dstats[i]->r_merged = r_merged;
      curr_dstats[i]->r_sectors = r_sectors;
      curr_dstats[i]->r_spent = r_spent;
      curr_dstats[i]->w_completed = w_completed;
      curr_dstats[i]->w_merged = w_merged;
      curr_dstats[i]->w_sectors = w_sectors;
      curr_dstats[i]->w_spent = w_spent;
      curr_dstats[i]->io_in_progress = io_in_progress;
      curr_dstats[i]->t_spent = t_spent;
      curr_dstats[i]->t_weighted = t_weighted;

      //! Save uptime.
      curr_dstats[i]->uptime = curr_uptime;

      i++;
    }
    fclose(diskstats_fp);

    //! Just Assert.
    assert(prev_dstats != NULL);

    unsigned long long perv_uptime = prev_dstats[0]->uptime;

    unsigned long long itv = get_interval(perv_uptime, curr_uptime);

    for (i = 0; i < n_dev; i++) {
      xds[i]->util =
          S_VALUE(prev_dstats[i]->t_spent, curr_dstats[i]->t_spent, itv);
      xds[i]->await =
          ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) -
           (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed))
              ? ((curr_dstats[i]->r_spent - prev_dstats[i]->r_spent) +
                 (curr_dstats[i]->w_spent - prev_dstats[i]->w_spent)) /
                    ((double) ((curr_dstats[i]->r_completed +
                                curr_dstats[i]->w_completed) -
                               (prev_dstats[i]->r_completed +
                                prev_dstats[i]->w_completed)))
              : 0.0;
      xds[i]->arqsz =
          ((curr_dstats[i]->r_completed + curr_dstats[i]->w_completed) -
           (prev_dstats[i]->r_completed + prev_dstats[i]->w_completed))
              ? ((curr_dstats[i]->r_sectors - prev_dstats[i]->r_sectors) +
                 (curr_dstats[i]->w_sectors - prev_dstats[i]->w_sectors)) /
                    ((double) ((curr_dstats[i]->r_completed +
                                curr_dstats[i]->w_completed) -
                               (prev_dstats[i]->r_completed +
                                prev_dstats[i]->w_completed)))
              : 0.0;

      r_await[i] = (curr_dstats[i]->r_completed - prev_dstats[i]->r_completed)
                       ? (curr_dstats[i]->r_spent - prev_dstats[i]->r_spent) /
                             ((double) (curr_dstats[i]->r_completed -
                                        prev_dstats[i]->r_completed))
                       : 0.0;
      w_await[i] = (curr_dstats[i]->w_completed - prev_dstats[i]->w_completed)
                       ? (curr_dstats[i]->w_spent - prev_dstats[i]->w_spent) /
                             ((double) (curr_dstats[i]->w_completed -
                                        prev_dstats[i]->w_completed))
                       : 0.0;
    }

    // for (i = 0; i < n_dev; i++) {
    //   printf("%s\t\t", curr_dstats[i]->devname);
    //   printf("%8.2f%8.2f",
    //          S_VALUE(prev_dstats[i]->r_merged, curr_dstats[i]->r_merged,
    //          itv), S_VALUE(prev_dstats[i]->w_merged,
    //          curr_dstats[i]->w_merged, itv));
    //   printf("%9.2f%9.2f",
    //          S_VALUE(prev_dstats[i]->r_completed,
    //          curr_dstats[i]->r_completed,
    //                  itv),
    //          S_VALUE(prev_dstats[i]->w_completed,
    //          curr_dstats[i]->w_completed,
    //                  itv));
    //   printf(
    //       "%9.2f%9.2f%9.2f%9.2f",
    //       S_VALUE(prev_dstats[i]->r_sectors, curr_dstats[i]->r_sectors, itv)
    //       /
    //           2048,
    //       S_VALUE(prev_dstats[i]->w_sectors, curr_dstats[i]->w_sectors, itv)
    //       /
    //           2048,
    //       xds[i]->arqsz,
    //       S_VALUE(prev_dstats[i]->t_weighted, curr_dstats[i]->t_weighted,
    //       itv) /
    //           1000.0);
    //   printf("%10.2f%10.2f%10.2f", xds[i]->await, r_await[i], w_await[i]);
    //   printf("%8.2f", xds[i]->util / 10.0);
    //   printf("\n");
    // }

    for (int i = 0; i < n_dev; ++i) {
      strcpy(iostats[i]->devname, curr_dstats[i]->devname);
      iostats[i]->rrqm_s =
          S_VALUE(prev_dstats[i]->r_merged, curr_dstats[i]->r_merged, itv);
      iostats[i]->wrqm_s =
          S_VALUE(prev_dstats[i]->w_merged, curr_dstats[i]->w_merged, itv);
      iostats[i]->r_s = S_VALUE(prev_dstats[i]->r_completed,
                                curr_dstats[i]->r_completed, itv);
      iostats[i]->w_s = S_VALUE(prev_dstats[i]->w_completed,
                                curr_dstats[i]->w_completed, itv);
      iostats[i]->rmb_s =
          S_VALUE(prev_dstats[i]->r_sectors, curr_dstats[i]->r_sectors, itv) /
          2048;
      iostats[i]->wmb_s =
          S_VALUE(prev_dstats[i]->w_sectors, curr_dstats[i]->w_sectors, itv) /
          2048;
      iostats[i]->avgrq_sz = xds[i]->arqsz;
      iostats[i]->avgqu_sz =
          S_VALUE(prev_dstats[i]->t_weighted, curr_dstats[i]->t_weighted, itv) /
          1000.0;
      iostats[i]->await = xds[i]->await;
      iostats[i]->r_await = r_await[i];
      iostats[i]->w_await = w_await[i];
      iostats[i]->until = xds[i]->util / 10;
    }
  }

  inline void print_iostats_csvtitle(std::ofstream &csv_stream) {
    csv_stream << "timestamp,devname,rrqm/s,wrqm/s,r/s,w/s,rMB/s,wMB/"
                  "s,avgrq-sz,avgqu-sz,await,r_await,w_await,util\n";
  }

  inline void print_iostats_csv_line(std::ofstream &csv_stream, int n_dev,
                                     diskann::iostats **iostats,
                                     char *target_dev, long long timestamp) {
    for (int i = 0; i < n_dev; ++i) {
      if (strcmp(iostats[i]->devname, target_dev) != 0) {
        continue;
      }

      csv_stream << timestamp << "," << iostats[i]->devname << ","
                 << iostats[i]->rrqm_s << "," << iostats[i]->wrqm_s << ","
                 << iostats[i]->r_s << "," << iostats[i]->w_s << ","
                 << iostats[i]->rmb_s << "," << iostats[i]->wmb_s << ","
                 << iostats[i]->avgrq_sz << "," << iostats[i]->avgqu_sz << ","
                 << iostats[i]->await << "," << iostats[i]->r_await << ","
                 << iostats[i]->w_await << "," << iostats[i]->until << "\n";
    }
  }

}  // namespace diskann