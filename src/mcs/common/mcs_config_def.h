//
// Created by root on 1/11/23.
//

/// The duration between dumping debug info to logs, or 0 to disable.
MCS_CONFIG(uint64_t, debug_dump_period_milliseconds, 10000)

/// Whether to enable Mcs event stats collection.
MCS_CONFIG(bool, event_stats, true)

/// Whether to enbale Mcs event stats metrics export.
/// Note that enabling this adds high overhead to
/// Mcs metrics agent.
MCS_CONFIG(bool, event_stats_metrics, false)

/// Whether to enable Mcs legacy scheduler warnings. These are replaced by
/// autoscaler messages after https://github.com/mcs-project/mcs/pull/18724.
/// TODO(ekl) remove this after Mcs 1.8
MCS_CONFIG(bool, legacy_scheduler_warnings, false)

/// The interval of periodic event loop stats print.
/// -1 means the feature is disabled. In this case, stats are available to
/// debug_state_*.txt
/// NOTE: This requires event_stats=1.
MCS_CONFIG(int64_t, event_stats_print_interval_ms, 60000)

/// In theory, this is used to detect Mcs cookie mismatches.
/// This magic number (hex for "MCS") is used instead of zero, rationale is
/// that it could still be possible that some random program sends an int64_t
/// which is zero, but it's much less likely that a program sends this
/// particular magic number.
MCS_CONFIG(int64_t, mcs_cookie, 0x5241590000000000)

/// The duration that a single handler on the event loop can take before a
/// warning is logged that the handler is taking too long.
MCS_CONFIG(int64_t, handler_warning_timeout_ms, 1000)

/// The duration between heartbeats sent by the mcslets.
MCS_CONFIG(uint64_t, mcslet_heartbeat_period_milliseconds, 1000)

/// The duration between loads pulled by GCS
MCS_CONFIG(uint64_t, gcs_pull_resource_loads_period_milliseconds, 1000)

/// If a component has not sent a heartbeat in the last num_heartbeats_timeout
/// heartbeat intervals, the mcslet monitor process will report
/// it as dead to the db_client table.
MCS_CONFIG(int64_t, num_heartbeats_timeout, 30)

/// If GCS restarts, before the first heatbeat is sent,
/// gcs_failover_worker_reconnect_timeout is used for the threshold
/// of the mcslet. This is very useful given that mcslet might need
/// a while to reconnect to the GCS, for example, when GCS is available
/// but not reachable to mcslet.
MCS_CONFIG(int64_t, gcs_failover_worker_reconnect_timeout, 120)

/// For a mcslet, if the last heartbeat was sent more than this many
/// heartbeat periods ago, then a warning will be logged that the heartbeat
/// handler is drifting.
MCS_CONFIG(uint64_t, num_heartbeats_warning, 5)

/// The duration between reporting resources sent by the mcslets.
MCS_CONFIG(uint64_t, mcslet_report_resources_period_milliseconds, 100)

/// The duration between mcslet check memory pressure and send gc request
MCS_CONFIG(uint64_t, mcslet_check_gc_period_milliseconds, 100)

/// Threshold when the node is beyond the memory capacity. If the memory is above the
/// memory_usage_threshold and free space is below the min_memory_free_bytes then
/// it will start killing processes to free up the space.
/// Ranging from [0, 1]
MCS_CONFIG(float, memory_usage_threshold, 0.95)

/// The interval between runs of the memory usage monitor.
/// Monitor is disabled when this value is 0.
MCS_CONFIG(uint64_t, memory_monitor_refresh_ms, 250)

/// The minimum amount of free space. If the memory is above the
/// memory_usage_threshold and free space is below min_memory_free_bytes then it
/// will start killing processes to free up the space. Disabled if it is -1.
///
/// This value is useful for larger host where the memory_usage_threshold could
/// represent a large chunk of memory, e.g. a host with 64GB of memory and 0.9 threshold
/// means 6.4 GB of the memory will not be usable.
MCS_CONFIG(int64_t, min_memory_free_bytes, (int64_t)-1)

/// The TTL for when the task failure entry is considered
/// eligble for garbage colletion.
MCS_CONFIG(uint64_t, task_failure_entry_ttl_ms, 15 * 60 * 1000)

/// The number of retries for the task or actor when
/// it fails due to the process being killed when the memory is running low on the node.
/// The process killing is done by memory monitor, which is enabled via
/// memory_monitor_refresh_ms. If the task or actor is not retriable then this value is
/// ignored. This retry counter is only used when the process is killed due to memory, and
/// the retry counter of the task or actor is only used when it fails in other ways
/// that is not related to running out of memory. Note infinite retry (-1) is not
/// supported.
MCS_CONFIG(uint64_t, task_oom_retries, 15)

/// If the mcslet fails to get agent info, we will retry after this interval.
MCS_CONFIG(uint64_t, mcslet_get_agent_info_interval_ms, 1)

/// For a mcslet, if the last resource report was sent more than this many
/// report periods ago, then a warning will be logged that the report
/// handler is drifting.
MCS_CONFIG(uint64_t, num_resource_report_periods_warning, 5)

/// Whether to report placement or regular resource usage for an actor.
/// Reporting placement may cause the autoscaler to overestimate the resources
/// required of the cluster, but reporting regular resource may lead to no
/// autoscaling when an actor can't be placed.
/// https://github.com/mcs-project/mcs/issues/26806
MCS_CONFIG(bool, report_actor_placement_resources, true)

/// Whether to record the creation sites of object references. This adds more
/// information to `mcs memory`, but introduces a little extra overhead when
/// creating object references (e.g. 5~10 microsec per call in Python).
/// TODO: maybe group this under MCS_DEBUG.
MCS_CONFIG(bool, record_ref_creation_sites, false)

/// Objects that have been unpinned are
/// added to a local cache. When the cache is flushed, all objects in the cache
/// will be eagerly evicted in a batch by freeing all plasma copies in the
/// cluster. If set, then this is the duration between attempts to flush the
/// local cache. If this is set to 0, then the objects will be freed as soon as
/// they enter the cache. To disable eager eviction, set this to -1.
/// NOTE(swang): The timer is checked by the mcslet during every heartbeat, so
/// this should be set to a value larger than
/// mcslet_heartbeat_period_milliseconds.
MCS_CONFIG(int64_t, free_objects_period_milliseconds, 1000)

/// Objects that have been unpinned are
/// added to a local cache. When the cache is flushed, all objects in the cache
/// will be eagerly evicted in a batch by freeing all plasma copies in the
/// cluster. This is the maximum number of objects in the local cache before it
/// is flushed. To disable eager eviction, set free_objects_period_milliseconds
/// to -1.
MCS_CONFIG(size_t, free_objects_batch_size, 100)

/// Whether to pin object lineage, i.e. the task that created the object and
/// the task's recursive dependencies. If this is set to true, then the system
/// will attempt to reconstruct the object from its lineage if the object is
/// lost.
MCS_CONFIG(bool, lineage_pinning_enabled, true)

/// Objects that require recovery are added to a local cache. This is the
/// duration between attempts to flush and recover the objects in the local
/// cache.
MCS_CONFIG(int64_t, reconstruct_objects_period_milliseconds, 100)

/// Maximum amount of lineage to keep in bytes. This includes the specs of all
/// tasks that have previously already finished but that may be retried again.
/// If we reach this limit, 50% of the current lineage will be evicted and
/// objects that are still in scope will no longer be reconstructed if lost.
/// Each task spec is on the order of 1KB but can be much larger if it has many
/// inlined args.
MCS_CONFIG(int64_t, max_lineage_bytes, 1024 * 1024 * 1024)

/// Whether to re-populate plasma memory. This avoids memory allocation failures
/// at runtime (SIGBUS errors creating new objects), however it will use more memory
/// upfront and can slow down Mcs startup.
/// See also: https://github.com/mcs-project/mcs/issues/14182
MCS_CONFIG(bool, preallocate_plasma_memory, false)

// If true, we place a soft cap on the numer of scheduling classes, see
// `worker_cap_initial_backoff_delay_ms`.
MCS_CONFIG(bool, worker_cap_enabled, true)

/// We place a soft cap on the number of tasks of a given scheduling class that
/// can run at once to limit the total nubmer of worker processes. After the
/// specified interval, the new task above that cap is allowed to run. The time
/// before the next tasks (above the cap) are allowed to run increases
/// exponentially. The soft cap is needed to prevent deadlock in the case where
/// a task begins to execute and tries to `mcs.get` another task of the same
/// class.
MCS_CONFIG(int64_t, worker_cap_initial_backoff_delay_ms, 1000)

/// After reaching the worker cap, the backoff delay will grow exponentially,
/// until it hits a maximum delay.
MCS_CONFIG(int64_t, worker_cap_max_backoff_delay_ms, 1000 * 10)

/// The fraction of resource utilization on a node after which the scheduler starts
/// to prefer spreading tasks to other nodes. This balances between locality and
/// even balancing of load. Low values (min 0.0) encourage more load spreading.
MCS_CONFIG(float, scheduler_spread_threshold, 0.5)

/// Whether to only report the usage of pinned copies of objects in the
/// object_store_memory resource. This means nodes holding secondary copies only
/// will become eligible for removal in the autoscaler.
MCS_CONFIG(bool, scheduler_report_pinned_bytes_only, true)

// The max allowed size in bytes of a return object from direct actor calls.
// Objects larger than this size will be spilled/promoted to plasma.
MCS_CONFIG(int64_t, max_direct_call_object_size, 100 * 1024)

// The max gRPC message size (the gRPC internal default is 4MB). We use a higher
// limit in Mcs to avoid crashing with many small inlined task arguments.
// Keep in sync with GCS_STORAGE_MAX_SIZE in packaging.py.
MCS_CONFIG(int64_t, max_grpc_message_size, 500 * 1024 * 1024)

// Retry timeout for trying to create a gRPC server. Only applies if the number
// of retries is non zero.
MCS_CONFIG(int64_t, grpc_server_retry_timeout_milliseconds, 1000)

// Whether to allow HTTP proxy on GRPC clients. Disable HTTP proxy by default since it
// disrupts local connections. Note that this config item only controls GrpcClient in
// `src/mcs/rpc/grpc_client.h`. Python GRPC clients are not directly controlled by this.
// NOTE (kfstorm): DO NOT set this config item via `_system_config`, use
// `MCS_grpc_enable_http_proxy` environment variable instead so that it can be passed to
// non-C++ children processes such as dashboard agent.
MCS_CONFIG(bool, grpc_enable_http_proxy, false)

// The min number of retries for direct actor creation tasks. The actual number
// of creation retries will be MAX(actor_creation_min_retries, max_restarts).
MCS_CONFIG(uint64_t, actor_creation_min_retries, 3)

/// Warn if more than this many tasks are queued for submission to an actor.
/// It likely indicates a bug in the user code.
MCS_CONFIG(uint64_t, actor_excess_queueing_warn_threshold, 5000)

/// When trying to resolve an object, the initial period that the mcslet will
/// wait before contacting the object's owner to check if the object is still
/// available. This is a lower bound on the time to report the loss of an
/// object stored in the distributed object store in the case that the worker
/// that created the original ObjectRef dies.
MCS_CONFIG(int64_t, object_timeout_milliseconds, 100)

/// The maximum duration that workers can hold on to another worker's lease
/// for direct task submission until it must be returned to the mcslet.
MCS_CONFIG(int64_t, worker_lease_timeout_milliseconds, 500)

/// The interval at which the workers will check if their mcslet has gone down.
/// When this happens, they will kill themselves.
MCS_CONFIG(uint64_t, mcslet_death_check_interval_milliseconds, 1000)

/// These are used by the worker to set timeouts and to batch requests when
/// getting objects.
MCS_CONFIG(int64_t, get_timeout_milliseconds, 1000)
MCS_CONFIG(int64_t, worker_get_request_size, 10000)
MCS_CONFIG(int64_t, worker_fetch_request_size, 10000)
/// How long to wait for a fetch to complete during mcs.get before warning the
/// user.
MCS_CONFIG(int64_t, fetch_warn_timeout_milliseconds, 60000)

/// How long to wait for a fetch before timing it out and throwing an error to
/// the user. This error should only be seen if there is extreme pressure on
/// the object directory, or if there is a bug in either object recovery or the
/// object directory.
MCS_CONFIG(int64_t, fetch_fail_timeout_milliseconds, 600000)

/// Temporary workaround for https://github.com/mcs-project/mcs/pull/16402.
MCS_CONFIG(bool, yield_plasma_lock_workaround, true)

// Whether to inline object status in serialized references.
// See https://github.com/mcs-project/mcs/issues/16025 for more details.
MCS_CONFIG(bool, inline_object_status_in_refs, true)

/// Number of times mcslet client tries connecting to a mcslet.
MCS_CONFIG(int64_t, mcslet_client_num_connect_attempts, 10)
MCS_CONFIG(int64_t, mcslet_client_connect_timeout_milliseconds, 1000)

/// The duration that the mcslet will wait before reinitiating a
/// fetch request for a missing task dependency. This time may adapt based on
/// the number of missing task dependencies.
MCS_CONFIG(int64_t, mcslet_fetch_timeout_milliseconds, 1000)

/// The duration that we wait after sending a worker SIGTERM before sending
/// the worker SIGKILL.
MCS_CONFIG(int64_t, kill_worker_timeout_milliseconds, 100)

/// The duration that we wait after the worker is launched before the
/// starting_worker_timeout_callback() is called.
MCS_CONFIG(int64_t, worker_register_timeout_seconds, 60)

/// The maximum number of workers to iterate whenever we analyze the resources usage.
MCS_CONFIG(uint32_t, worker_max_resource_analysis_iteration, 128)

/// A value to add to workers' OOM score adjustment, so that the OS prioritizes
/// killing these over the mcslet. 0 or positive values only (negative values
/// require sudo permissions).
/// NOTE(swang): Linux only.
MCS_CONFIG(int, worker_oom_score_adjustment, 1000)

/// Sets workers' nice value on posix systems, so that the OS prioritizes CPU for other
/// processes over worker. This makes CPU available to GCS, Mcslet and user processes
/// even when workers are busy.
/// Valid value is [0, 19] (negative values require sudo permissions).
/// NOTE: Linux, Unix and MacOS only.
MCS_CONFIG(int, worker_niceness, 15)

/// Allow up to 60 seconds for connecting to Redis.
MCS_CONFIG(int64_t, redis_db_connect_retries, 600)
MCS_CONFIG(int64_t, redis_db_connect_wait_milliseconds, 100)

/// The object manager's global timer interval in milliseconds.
MCS_CONFIG(int, object_manager_timer_freq_ms, 100)

/// Timeout, in milliseconds, to wait before retrying a failed pull in the
/// ObjectManager.
MCS_CONFIG(int, object_manager_pull_timeout_ms, 10000)

/// Timeout, in milliseconds, to wait until the Push request fails.
/// Special value:
/// Negative: waiting infinitely.
/// 0: giving up retrying immediately.
MCS_CONFIG(int, object_manager_push_timeout_ms, 10000)

/// Default chunk size for multi-chunk transfers to use in the object manager.
/// In the object manager, no single thread is permitted to transfer more
/// data than what is specified by the chunk size unless the number of object
/// chunks exceeds the number of available sending threads.
/// NOTE(ekl): this has been raised to lower broadcast overheads.
MCS_CONFIG(uint64_t, object_manager_default_chunk_size, 5 * 1024 * 1024)

/// The maximum number of outbound bytes to allow to be outstanding. This avoids
/// excessive memory usage during object broadcast to many receivers.
MCS_CONFIG(uint64_t,
        object_manager_max_bytes_in_flight,
((uint64_t)2) * 1024 * 1024 * 1024)

/// Maximum number of ids in one batch to send to GCS to delete keys.
MCS_CONFIG(uint32_t, maximum_gcs_deletion_batch_size, 1000)

/// Maximum number of items in one batch to scan/get/delete from GCS storage.
MCS_CONFIG(uint32_t, maximum_gcs_storage_operation_batch_size, 1000)

/// When getting objects from object store, max number of ids to print in the warning
/// message.
MCS_CONFIG(uint32_t, object_store_get_max_ids_to_print_in_warning, 20)
/// Number of threads used by rpc server in gcs server.
MCS_CONFIG(uint32_t,
        gcs_server_rpc_server_thread_num,
        std::max(1U, std::thread::hardware_concurrency() / 4U))
/// Number of threads used by rpc server in gcs server.
MCS_CONFIG(uint32_t,
        gcs_server_rpc_client_thread_num,
        std::max(1U, std::thread::hardware_concurrency() / 4U))
/// Allow up to 5 seconds for connecting to gcs service.
/// Note: this only takes effect when gcs service is enabled.
MCS_CONFIG(int64_t, gcs_service_connect_retries, 50)
/// Waiting time for each gcs service connection.
MCS_CONFIG(int64_t, internal_gcs_service_connect_wait_milliseconds, 100)
/// The interval at which the gcs server will check if redis has gone down.
/// When this happens, gcs server will kill itself.
MCS_CONFIG(uint64_t, gcs_redis_heartbeat_interval_milliseconds, 100)
/// Duration to wait between retries for leasing worker in gcs server.
MCS_CONFIG(uint32_t, gcs_lease_worker_retry_interval_ms, 200)
/// Duration to wait between retries for creating actor in gcs server.
MCS_CONFIG(uint32_t, gcs_create_actor_retry_interval_ms, 200)
/// Exponential backoff params for gcs to retry creating a placement group
MCS_CONFIG(uint64_t, gcs_create_placement_group_retry_min_interval_ms, 100)
MCS_CONFIG(uint64_t, gcs_create_placement_group_retry_max_interval_ms, 1000)
MCS_CONFIG(double, gcs_create_placement_group_retry_multiplier, 1.5)
/// Maximum number of destroyed actors in GCS server memory cache.
MCS_CONFIG(uint32_t, maximum_gcs_destroyed_actor_cached_count, 100000)
/// Maximum number of dead nodes in GCS server memory cache.
MCS_CONFIG(uint32_t, maximum_gcs_dead_node_cached_count, 1000)
// The interval at which the gcs server will pull a new resource.
MCS_CONFIG(int, gcs_resource_report_poll_period_ms, 100)
// The number of concurrent polls to polls to GCS.
MCS_CONFIG(uint64_t, gcs_max_concurrent_resource_pulls, 100)
// The storage backend to use for the GCS. It can be either 'redis' or 'memory'.
MCS_CONFIG(std::string, gcs_storage, "memory")

/// Duration to sleep after failing to put an object in plasma because it is full.
MCS_CONFIG(uint32_t, object_store_full_delay_ms, 10)

/// The threshold to trigger a global gc
MCS_CONFIG(double, high_plasma_storage_usage, 0.7)

/// The amount of time between automatic local Python GC triggers.
MCS_CONFIG(uint64_t, local_gc_interval_s, 10 * 60)

/// The min amount of time between local GCs (whether auto or mem pressure triggered).
MCS_CONFIG(uint64_t, local_gc_min_interval_s, 10)

/// The min amount of time between triggering global_gc in mcslet. This only applies
/// to global GCs triggered due to high_plasma_storage_usage.
MCS_CONFIG(uint64_t, global_gc_min_interval_s, 30)

/// Duration to wait between retries for failed tasks.
MCS_CONFIG(uint32_t, task_retry_delay_ms, 0)

/// The base retry delay for exponential backoff when the task fails due to OOM.
/// No delay if this value is zero.
MCS_CONFIG(uint32_t, task_oom_retry_delay_base_ms, 1000)

/// Duration to wait between retrying to kill a task.
MCS_CONFIG(uint32_t, cancellation_retry_ms, 2000)

/// Whether to start a background thread to import Python dependencies eagerly.
/// When set to false, Python dependencies will still be imported, only when
/// they are needed.
MCS_CONFIG(bool, start_python_importer_thread, true)

/// Determines if forking in Mcs actors / tasks are supported.
/// Note that this only enables forking in workers, but not drivers.
MCS_CONFIG(bool, support_fork, false)

/// Maximum timeout for GCS reconnection in seconds.
/// Each reconnection ping will be retried every 1 second.
MCS_CONFIG(int32_t, gcs_rpc_server_reconnect_timeout_s, 60)

/// The timeout for GCS connection in seconds
MCS_CONFIG(int32_t, gcs_rpc_server_connect_timeout_s, 5)

/// Minimum interval between reconnecting gcs rpc server when gcs server restarts.
MCS_CONFIG(int32_t, minimum_gcs_reconnect_interval_milliseconds, 5000)

/// gRPC channel reconnection related configs to GCS.
/// Check https://grpc.github.io/grpc/core/group__grpc__arg__keys.html for details
MCS_CONFIG(int32_t, gcs_grpc_max_reconnect_backoff_ms, 2000)
MCS_CONFIG(int32_t, gcs_grpc_min_reconnect_backoff_ms, 100)
MCS_CONFIG(int32_t, gcs_grpc_initial_reconnect_backoff_ms, 100)

/// Maximum bytes of request queued when RPC failed due to GCS is down.
/// If reach the limit, the core worker will hang until GCS is reconnected.
/// By default, the value if 5GB.
MCS_CONFIG(uint64_t, gcs_grpc_max_request_queued_max_bytes, 1024UL * 1024 * 1024 * 5)

/// The duration between two checks for grpc status.
MCS_CONFIG(int32_t, gcs_client_check_connection_status_interval_milliseconds, 1000)

/// Feature flag to use the mcs syncer for resource synchronization
MCS_CONFIG(bool, use_mcs_syncer, false)
/// Due to the protocol drawback, mcslet needs to refresh the message if
/// no message is received for a while.
/// Refer to https://tinyurl.com/n6kvsp87 for more details
MCS_CONFIG(int64_t, mcs_syncer_message_refresh_interval_ms, 3000)

/// The queuing buffer of mcs syncer. This indicates how many concurrent
/// requests can run in flight for syncing.
MCS_CONFIG(int64_t, mcs_syncer_polling_buffer, 5)

/// The interval at which the gcs client will check if the address of gcs service has
/// changed. When the address changed, we will resubscribe again.
MCS_CONFIG(uint64_t, gcs_service_address_check_interval_milliseconds, 1000)

/// The batch size for metrics export.
MCS_CONFIG(int64_t, metrics_report_batch_size, 100)

/// The interval duration for which task state events will be reported to GCS.
/// The reported data should only be used for observability.
/// Setting the value to 0 disables the task event recording and reporting.
MCS_CONFIG(int64_t, task_events_report_interval_ms, 1000)

/// The number of tasks tracked in GCS for task state events. Any additional events
/// from new tasks will evict events of tasks reported earlier.
/// Setting the value to -1 allows for unlimited task events stored in GCS.
MCS_CONFIG(int64_t, task_events_max_num_task_in_gcs, 100000)

/// Max number of task events stored in the buffer on workers. Any additional events
/// will be dropped.
/// Setting the value to -1 allows for unlimited task events buffered on workers.
MCS_CONFIG(int64_t, task_events_max_num_task_events_in_buffer, 10000)

/// Max number of profile events allowed for a single task when sent to GCS.
/// NOTE: this limit only applies to the profile events per task in a single
/// report gRPC call. A task could have more profile events in GCS from multiple
/// report gRPC call.
/// Setting the value to -1 allows unlimited profile events to be sent.
MCS_CONFIG(int64_t, task_events_max_num_profile_events_for_task, 100)

/// Whether or not we enable metrics collection.
MCS_CONFIG(bool, enable_metrics_collection, true)

// Max number bytes of inlined objects in a task rpc request/response.
MCS_CONFIG(int64_t, task_rpc_inlined_bytes_limit, 10 * 1024 * 1024)

/// Maximum number of pending lease requests per scheduling category
MCS_CONFIG(uint64_t, max_pending_lease_requests_per_scheduling_category, 10)

/// Wait timeout for dashboard agent register.
#ifdef _WIN32
// agent startup time can involve creating conda environments
MCS_CONFIG(uint32_t, agent_register_timeout_ms, 100 * 1000)
#else
MCS_CONFIG(uint32_t, agent_register_timeout_ms, 30 * 1000)
#endif

/// If the agent manager fails to communicate with the dashboard agent, we will retry
/// after this interval.
MCS_CONFIG(uint32_t, agent_manager_retry_interval_ms, 1000)

/// The maximum number of resource shapes included in the resource
/// load reported by each mcslet.
MCS_CONFIG(int64_t, max_resource_shapes_per_load_report, 100)

/// The timeout for synchronous GCS requests in seconds.
MCS_CONFIG(int64_t, gcs_server_request_timeout_seconds, 60)

/// Whether to enable worker prestarting: https://github.com/mcs-project/mcs/issues/12052
MCS_CONFIG(bool, enable_worker_prestart, true)

/// The interval of periodic idle worker killing. Value of 0 means worker capping is
/// disabled.
MCS_CONFIG(uint64_t, kill_idle_workers_interval_ms, 200)

/// The idle time threshold for an idle worker to be killed.
MCS_CONFIG(int64_t, idle_worker_killing_time_threshold_ms, 1000)

/// The soft limit of the number of workers.
/// -1 means using num_cpus instead.
MCS_CONFIG(int64_t, num_workers_soft_limit, -1)

// The interval where metrics are exported in milliseconds.
MCS_CONFIG(uint64_t, metrics_report_interval_ms, 10000)

/// Enable the task timeline. If this is enabled, certain events such as task
/// execution are profiled and sent to the GCS.
/// This requires MCS_task_events_report_interval_ms > 0, so that events will
/// be sent to GCS.
MCS_CONFIG(bool, enable_timeline, true)

/// The maximum number of pending placement group entries that are reported to monitor to
/// autoscale the cluster.
MCS_CONFIG(int64_t, max_placement_group_load_report_size, 1000)

/* Configuration parameters for object spilling. */
/// JSON configuration that describes the external storage. This is passed to
/// Python IO workers to determine how to store/restore an object to/from
/// external storage.
MCS_CONFIG(std::string, object_spilling_config, "")

/// Log an ERROR-level message about spilling every time this amount of bytes has been
/// spilled, with exponential increase in interval. This can be set to zero to disable.
MCS_CONFIG(int64_t, verbose_spill_logs, 2L * 1024 * 1024 * 1024)

/// Whether to enable automatic object spilling. If enabled, then
/// Mcs will choose objects to spill when the object store is out of
/// memory.
MCS_CONFIG(bool, automatic_object_spilling_enabled, true)

/// The maximum number of I/O worker that mcslet starts.
MCS_CONFIG(int, max_io_workers, 4)

/// Mcs's object spilling fuses small objects into a single file before flushing them
/// to optimize the performance.
/// The minimum object size that can be spilled by each spill operation. 100 MB by
/// default. This value is not recommended to set beyond --object-store-memory.
MCS_CONFIG(int64_t, min_spilling_size, 100 * 1024 * 1024)

/// If set to less than 1.0, Mcs will start spilling objects when existing objects
/// take more than this percentage of the available memory.
MCS_CONFIG(float, object_spilling_threshold, 0.8)

/// Maximum number of objects that can be fused into a single file.
MCS_CONFIG(int64_t, max_fused_object_count, 2000)

/// Grace period until we throw the OOM error to the application in seconds.
/// In unlimited allocation mode, this is the time delay prior to fallback allocating.
MCS_CONFIG(int64_t, oom_grace_period_s, 2)

/// Whether or not the external storage is the local file system.
/// Note that this value should be overridden based on the storage type
/// specified by object_spilling_config.
MCS_CONFIG(bool, is_external_storage_type_fs, true)

/// Control the capacity threshold for mcs local file system (for object store).
/// Once we are over the capacity, all subsequent object creation will fail.
MCS_CONFIG(float, local_fs_capacity_threshold, 0.95)

/// Control the frequency of checking the disk usage.
MCS_CONFIG(uint64_t, local_fs_monitor_interval_ms, 100)

/* Configuration parameters for locality-aware scheduling. */
/// Whether to enable locality-aware leasing. If enabled, then Mcs will consider task
/// dependency locality when choosing a worker for leasing.
MCS_CONFIG(bool, locality_aware_leasing_enabled, true)

/* Configuration parameters for logging */
/// Parameters for log rotation. This value is equivalent to RotatingFileHandler's
/// maxBytes argument.
MCS_CONFIG(int64_t, log_rotation_max_bytes, 100 * 1024 * 1024)

/// Parameters for log rotation. This value is equivalent to RotatingFileHandler's
/// backupCount argument.
MCS_CONFIG(int64_t, log_rotation_backup_count, 5)

/// When tasks that can't be sent because of network error. we'll never receive a DEAD
/// notification, in this case we'll wait for a fixed timeout value and then mark it
/// as failed.
MCS_CONFIG(int64_t, timeout_ms_task_wait_for_death_info, 1000)

/// The core worker heartbeat interval. During heartbeat, it'll
/// report the loads to mcslet.
MCS_CONFIG(int64_t, core_worker_internal_heartbeat_ms, 1000)

/// Maximum amount of memory that will be used by running tasks' args.
MCS_CONFIG(float, max_task_args_memory_fraction, 0.7)

/// The maximum number of objects to publish for each publish calls.
MCS_CONFIG(int, publish_batch_size, 5000)

/// Maximum size in bytes of buffered messages per entity, in Mcs publisher.
MCS_CONFIG(int, publisher_entity_buffer_max_bytes, 10 << 20)

/// The maximum command batch size.
MCS_CONFIG(int64_t, max_command_batch_size, 2000)

/// The maximum batch size for OBOD report.
MCS_CONFIG(int64_t, max_object_report_batch_size, 2000)

/// For Mcs publishers, the minimum time to drop an inactive subscriber connection in ms.
/// In the current implementation, a subscriber might be dead for up to 3x the configured
/// time before it is deleted from the publisher, i.e. deleted in 300s ~ 900s.
MCS_CONFIG(uint64_t, subscriber_timeout_ms, 300 * 1000)

// This is the minimum time an actor will remain in the actor table before
// being garbage collected when a job finishes
MCS_CONFIG(uint64_t, gcs_actor_table_min_duration_ms, /*  5 min */ 60 * 1000 * 5)

/// Whether to enable GCS-based actor scheduling.
MCS_CONFIG(bool, gcs_actor_scheduling_enabled, false)

MCS_CONFIG(uint32_t, max_error_msg_size_bytes, 512 * 1024)

/// If enabled, mcslet will report resources only when resources are changed.
MCS_CONFIG(bool, enable_light_weight_resource_report, true)

// The number of seconds to wait for the Mcslet to start. This is normally
// fast, but when MCS_preallocate_plasma_memory=1 is set, it may take some time
// (a few GB/s) to populate all the pages on Mcslet startup.
MCS_CONFIG(uint32_t,
        mcslet_start_wait_time_s,
        std::getenv("MCS_preallocate_plasma_memory") != nullptr &&
std::getenv("MCS_preallocate_plasma_memory") == std::string("1")
? 120
: 10)

/// The scheduler will treat these predefined resource types as unit_instance.
/// Default predefined_unit_instance_resources is "GPU".
/// When set it to "CPU,GPU", we will also treat CPU as unit_instance.
MCS_CONFIG(std::string, predefined_unit_instance_resources, "GPU")

/// The scheduler will treat these custom resource types as unit_instance.
/// Default custom_unit_instance_resources is empty.
/// When set it to "FPGA", we will treat FPGA as unit_instance.
MCS_CONFIG(std::string, custom_unit_instance_resources, "")

// Maximum size of the batches when broadcasting resources to mcslet.
MCS_CONFIG(uint64_t, resource_broadcast_batch_size, 512)

// Maximum mcs sync message batch size in bytes (1MB by default) between nodes.
MCS_CONFIG(uint64_t, max_sync_message_batch_bytes, 1 * 1024 * 1024)

// If enabled and worker stated in container, the container will add
// resource limit.
MCS_CONFIG(bool, worker_resource_limits_enabled, false)

// When enabled, workers will not be re-used across tasks requesting different
// resources (e.g., CPU vs GPU).
MCS_CONFIG(bool, isolate_workers_across_resource_types, true)

// When enabled, workers will not be re-used across tasks of different types
// (i.e., Actor vs normal tasks).
MCS_CONFIG(bool, isolate_workers_across_task_types, true)

/// ServerCall instance number of each RPC service handler
MCS_CONFIG(int64_t, gcs_max_active_rpcs_per_handler, 100)

/// grpc keepalive sent interval
/// This is only configured in GCS server now.
/// NOTE: It is not ideal for other components because
/// they have a failure model that considers network failures as component failures
/// and this configuration break that assumption. We should apply to every other component
/// after we change this failure assumption from code.
MCS_CONFIG(int64_t, grpc_keepalive_time_ms, 10000)

/// grpc keepalive timeout
MCS_CONFIG(int64_t, grpc_keepalive_timeout_ms, 20000)

/// Whether to use log reporter in event framework
MCS_CONFIG(bool, event_log_reporter_enabled, false)

/// Whether to enable register actor async.
/// If it is false, the actor registration to GCS becomes synchronous, i.e.,
/// core worker is blocked until GCS registers the actor and replies to it.
/// If it is true, the actor registration is async, but actor handles cannot
/// be passed to other worker until it is registered to GCS.
MCS_CONFIG(bool, actor_register_async, true)

/// Event severity threshold value
MCS_CONFIG(std::string, event_level, "warning")

/// Whether to avoid scheduling cpu requests on gpu nodes
MCS_CONFIG(bool, scheduler_avoid_gpu_nodes, true)

/// Whether to skip running local GC in runtime env.
MCS_CONFIG(bool, runtime_env_skip_local_gc, false)

/// The namespace for the storage.
/// This fields is used to isolate data stored in DB.
MCS_CONFIG(std::string, external_storage_namespace, "default")

/// Whether or not use TLS.
MCS_CONFIG(bool, USE_TLS, false)

/// Location of TLS credentials
MCS_CONFIG(std::string, TLS_SERVER_CERT, "")
MCS_CONFIG(std::string, TLS_SERVER_KEY, "")
MCS_CONFIG(std::string, TLS_CA_CERT, "")

/// Location of Redis TLS credentials
/// https://github.com/redis/hiredis/blob/c78d0926bf169670d15cfc1214e4f5d21673396b/README.md#hiredis-openssl-wrappers
MCS_CONFIG(std::string, REDIS_CA_CERT, "")
MCS_CONFIG(std::string, REDIS_CA_PATH, "")

MCS_CONFIG(std::string, REDIS_CLIENT_CERT, "")
MCS_CONFIG(std::string, REDIS_CLIENT_KEY, "")
MCS_CONFIG(std::string, REDIS_SERVER_NAME, "")

/// grpc delay testing flags
///  To use this, simply do
///      export MCS_testing_asio_delay_us="method1=min_val:max_val,method2=20:100"
//  The delay is a random number between the interval. If method equals '*',
//  it will apply to all methods.
MCS_CONFIG(std::string, testing_asio_delay_us, "")

/// A feature flag to enable pull based health check.
MCS_CONFIG(bool, pull_based_healthcheck, true)
/// The following are configs for the health check. They are borrowed
/// from k8s health probe (shorturl.at/jmTY3)

/// The delay to send the first health check.
MCS_CONFIG(int64_t, health_check_initial_delay_ms, 5000)
/// The interval between two health check.
MCS_CONFIG(int64_t, health_check_period_ms, 3000)
/// The timeout for a health check.
MCS_CONFIG(int64_t, health_check_timeout_ms, 10000)
/// The threshold to consider a node dead.
MCS_CONFIG(int64_t, health_check_failure_threshold, 5)

/// The pool size for grpc server call.
MCS_CONFIG(int64_t,
        num_server_call_thread,
        std::max((int64_t)1, (int64_t)(std::thread::hardware_concurrency() / 4U)))

/// Use madvise to prevent worker/mcslet coredumps from including
/// the mapped plasma pages.
MCS_CONFIG(bool, worker_core_dump_exclude_plasma_store, true)
MCS_CONFIG(bool, mcslet_core_dump_exclude_plasma_store, true)
