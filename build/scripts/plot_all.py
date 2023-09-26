import pandas as pd
import matplotlib.pyplot as plt
import io

def plot_rMBs(csv_path, dst_path):
    # Read the CSV data into a pandas DataFrame
    df = pd.read_csv(csv_path)

    # Plot the 'rMB/s' column
    plt.figure(figsize=(10, 6))
    plt.plot(df['timestamp'], df['rMB/s'], marker='o', linestyle='-')
    plt.title('rMB/s over Time')
    plt.xlabel('Timestamp')
    plt.ylabel('rMB/s')
    plt.grid(True)
    plt.tight_layout()

    # Save the plot as an image
    plt.savefig(dst_path)

    # Close the plot
    plt.close()

def filter_data(df, label, condition_value):
    """Filter the DataFrame based on a condition."""
    return df[df[label] == condition_value]

def plot_condition(tmp_df, x_label, y_label, title, label_name, condition_values, dst_path):    
    # Initialize the figure and axis outside the loop
    plt.figure(figsize=(10, 6))

    for condition_value in condition_values:
        filtered_df = filter_data(tmp_df, label_name, condition_value)
        # Plot the 'QPS / thread' column for each L value
        plt.plot(filtered_df[x_label], filtered_df[y_label], marker='o', linestyle='-', label=f"{label_name}={condition_value}")

    # Set the title, labels, and other properties outside the loop
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.grid(True)
    plt.tight_layout()
    plt.legend()  # To show the legend indicating different L values
    plt.savefig(dst_path)


def plot_rMBs(base_path, num_points, threads_configures, Ls):
    # csv columns : timestamp,devname,rrqm/s,wrqm/s,r/s,w/s,rMB/s,wMB/s,avgrq-sz,avgqu-sz,await,r_await,w_await,util
    df = pd.DataFrame(columns=['# threads', 'timestamp', 'devname', 'rrqm/s', 'wrqm/s', 'r/s', 'w/s', 'rMB/s', 'wMB/s', 'avgrq-sz', 'avgqu-sz', 'await', 'r_await', 'w_await', 'util'])

    for num_point in num_points:
        # csv columns : timestamp,devname,rrqm/s,wrqm/s,r/s,w/s,rMB/s,wMB/s,avgrq-sz,avgqu-sz,await,r_await,w_await,util
        tmp_df = pd.DataFrame(columns=['# threads', 'timestamp', 'devname', 'rrqm/s', 'wrqm/s', 'r/s', 'w/s', 'rMB/s', 'wMB/s', 'avgrq-sz', 'avgqu-sz', 'await', 'r_await', 'w_await', 'util'])
        
        # Initialize the figure and axis outside the loop
        # plt.figure(figsize=(10, 6))
        for threads_configure in threads_configures:
            # Read the CSV data into a temporary DataFrame
            temp_df = pd.read_csv(base_path + str(num_point) + "/" + str(threads_configure) + "/search_res/res_iostats.csv")
            
            # Add the # threads column to the temporary DataFrame and populate it with the value from threads_configure
            temp_df['# threads'] = threads_configure
            
            # Append the temporary DataFrame to the main DataFrame
            tmp_df = pd.concat([tmp_df, temp_df], ignore_index=True)
        
        plot_condition(tmp_df, 'timestamp', 'rMB/s', 'rMB/s over timeline', '# threads',  threads_configures, base_path + str(num_point) + "/combined_rMBs_over_time.png")
        plot_condition(tmp_df, 'timestamp', 'wMB/s', 'wMB/s over timeline', '# threads', threads_configures, base_path + str(num_point) + "/combined_wMBs_over_time.png")
        
        df = pd.concat([df, tmp_df], ignore_index=True)

def plot_stats(base_path, num_points, threads_configures, Ls):
    # csv colums : L,Beamwidth,QPS / thread,Mean Latency,99.9 Latency,Mean IOs,CPU (us),ComputeDist (us),PQDist (us),SinglePQDist (us) & chunks,IO Time(us),Mean IO 4k,Mean Nnbrs,Mean Ncompdist,Recall@10
    df = pd.DataFrame(columns=['# threads', 'L', 'Beamwidth', 'QPS / thread', 'Mean Latency', 
                               '99.9 Latency', 'Mean IOs', 'CPU (us)', 'ComputeDist (us)', 
                               'PQDist (us)', 'SinglePQDist (us) & chunks', 'IO Time(us)', 
                               'Mean IO 4k', 'Mean Nnbrs', 'Mean Ncompdist', 'Recall@10'])
    for num_point in num_points:
        # csv colums : L,Beamwidth,QPS / thread,Mean Latency,99.9 Latency,Mean IOs,CPU (us),ComputeDist (us),PQDist (us),SinglePQDist (us) & chunks,IO Time(us),Mean IO 4k,Mean Nnbrs,Mean Ncompdist,Recall@10
        tmp_df = pd.DataFrame(columns=['# threads', 'L', 'Beamwidth', 'QPS / thread', 'Mean Latency', 
                                '99.9 Latency', 'Mean IOs', 'CPU (us)', 'ComputeDist (us)', 
                                'PQDist (us)', 'SinglePQDist (us) & chunks', 'IO Time(us)', 
                                'Mean IO 4k', 'Mean Nnbrs', 'Mean Ncompdist', 'Recall@10'])
        for threads_configure in threads_configures:
            # Read the CSV data into a temporary DataFrame
            temp_df = pd.read_csv(base_path + str(num_point) + "/" + str(threads_configure) + "/search_stats.csv")
            
            # Add the # threads column to the temporary DataFrame and populate it with the value from threads_configure
            temp_df['# threads'] = threads_configure
            
            # Append the temporary DataFrame to the main DataFrame
            tmp_df = pd.concat([tmp_df, temp_df], ignore_index=True)

            plot_condition(tmp_df, '# threads', 'QPS / thread', 'QPS / thread over # thread', 'L', Ls, base_path + str(num_point) + "/combined_QPS_over_threads.png")
            plot_condition(tmp_df, '# threads', 'Mean Latency', 'Mean Latency over # thread', 'L', Ls, base_path + str(num_point) + "/combined_Mean_Latency_over_threads.png")
            plot_condition(tmp_df, '# threads', '99.9 Latency', '99.9 Latency over # thread', 'L', Ls, base_path + str(num_point) + "/combined_99.9_Latency_over_threads.png")
            plot_condition(tmp_df, '# threads', 'Mean IOs', 'Mean IOs over # thread', 'L', Ls, base_path + str(num_point) + "/combined_Mean_IOs_over_threads.png")
            plot_condition(tmp_df, '# threads', 'CPU (us)', 'CPU (us) over # thread', 'L', Ls, base_path + str(num_point) + "/combined_CPU_over_threads.png")

        plot_condition(tmp_df, 'L', 'Recall@10', 'Recall@10 over # thread', '# threads', threads_configures, base_path + str(num_point) + "/combined_Recall@10_over_threads.png")

        df = pd.concat([df, tmp_df], ignore_index=True)
        

if __name__ == "__main__":
    Ls = [10, 20, 30, 40, 50, 100]
    threads_configures = [4, 8, 16, 32, 64]
    num_points = [100000, 200000, 300000, 400000, 500000, 1000000, 10000000, 100000000]
    base_path = "/app/DiskANN/build/data/bigann_threads_search/"

    # for num_point in num_points:
    #     for threads_configure in threads_configures:
    #         plot_rMBs(base_path + str(num_point) + "/" + str(threads_configure) + "/search_res/res_iostats.csv", base_path + str(num_point) + "/" + str(threads_configure) + "/search_res/rMBs_over_time.png")

    plot_stats(base_path, num_points, threads_configures, Ls)

    plot_rMBs(base_path, num_points, threads_configures, Ls)

