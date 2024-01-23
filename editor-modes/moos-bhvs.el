;*****************************************************************;
;*    NAME: Jacob Gerlach and Michael Benjamin                   *;
;*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     *;
;*    FILE: moos-bhvs.el                                         *;
;*    DATE: April 27th 2014                                      *;
;*                                                               *;
;* This file is part of MOOS-IvP                                 *;
;*                                                               *;
;* MOOS-IvP is free software: you can redistribute it and/or     *;
;* modify it under the terms of the GNU General Public License   *;
;* as published by the Free Software Foundation, either version  *;
;* 3 of the License, or (at your option) any later version.      *;
;*                                                               *;
;* MOOS-IvP is distributed in the hope that it will be useful,   *;
;* but WITHOUT ANY WARRANTY; without even the implied warranty   *;
;* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  *;
;* the GNU General Public License for more details.              *;
;*                                                               *;
;* You should have received a copy of the GNU General Public     *;
;* License along with MOOS-IvP.  If not, see                     *;
;* <http://www.gnu.org/licenses/>.                               *;
;*****************************************************************;

(setq moos-bhv-and-keyword-list
      (list
       '("BHV_AvdColregsV17" "post_mapping" "pwt_outer_dist" "pwt_inner_dist" "completed_dist" "min_util_cpa_dist" "max_util_cpa_dist" "turn_radius" "avoid_mode" "avoid_submode" "no_alert_request" "contact_type_required" "pwt_grade" "bearing_line_config" "contact" "on_no_contact_ok" "extrapolate" "decay" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region")

       '("BHV_AvdColregsV19" "post_mapping" "pwt_outer_dist" "pwt_inner_dist" "completed_dist" "min_util_cpa_dist" "max_util_cpa_dist" "turn_radius" "avoid_mode" "avoid_submode" "no_alert_request" "contact_type_required" "pwt_grade" "giveway_bow_dist" "use_refinery" "check_plateaus" "check_validity" "pcheck_thresh" "bearing_line_config" "contact" "on_no_contact_ok" "extrapolate" "decay" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region")

       '("BHV_AvdColregsV22" "post_mapping" "pwt_outer_dist" "pwt_inner_dist" "completed_dist" "min_util_cpa_dist" "max_util_cpa_dist" "turn_radius" "avoid_mode" "avoid_submode" "no_alert_request" "contact_type_required" "pwt_grade" "giveway_bow_dist" "use_refinery" "check_plateaus" "check_validity" "pcheck_thresh" "bearing_line_config" "contact" "on_no_contact_ok" "extrapolate" "decay" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region" "headon_only" "velocity_filter" )
       
       '("BHV_LegRun" "vx1" "vx2" "speed" "wpt_status_var" "wptflag"  "cycleflag" "legflag" "midflag" "warn_overshoot"  "lead" "lead_damper" "capture_line" "repeat"  "visual_hints" "radius" "capture_radius" "slip_radius" "midpct" "leg_length" "leg_length_mod" "patience" "turn1_dir" "turn1_rad" "turn1_bias" "turn2_dir" "turn2_rad" "turn2_bias" "turn_rad_min" "turn1_rad_mod" "turn2_rad_mod" "turn_rad_mod" "turn1_bias_mod" "turn2_bias_mod" "turn_bias_mod""mid_pct" "leg_spds" "shift_pt" "shift_point" "visual_hints" "turn_pt_gap")

       '("BHV_LegRunX" "vx1" "vx2" "speed" "wpt_status_var" "wptflag"  "cycleflag" "legflag" "midflag" "warn_overshoot"  "lead" "lead_damper" "capture_line" "repeat"  "visual_hints" "radius" "capture_radius" "slip_radius" "midpct" "leg_length" "leg_length_mod" "patience" "turn1_dir" "turn1_rad" "turn1_bias" "turn2_dir" "turn2_rad" "turn2_bias" "turn_rad_min" "turn1_rad_mod" "turn2_rad_mod" "turn_rad_mod" "turn1_bias_mod" "turn2_bias_mod" "turn_bias_mod""mid_pct" "leg_spds" "shift_pt" "shift_point" "visual_hints" "turn_pt_gap" "coord" "turn1_ext" "turn2_ext" "turn_ext" "turn1_ext_mod" "turn2_ext_mod" "turn_ext_mod" "adjust_turn" )

       '("BHV_LegRunZ" "pa" "p2" "vx1" "vx2" "leg" "full_leg" "center" "speed" "wpt_status_var" "wptflag"  "cycleflag" "legflag" "midflag" "warn_overshoot"  "lead" "lead_damper" "capture_line" "repeat"  "visual_hints" "radius" "capture_radius" "slip_radius" "midpct" "leg_len" "leg_length" "leg_angle" "leg_ang" "leg_ang_mod" "leg_angle_mod" "leg_len_mod" "leg_length_mod" "patience" "turn1_dir" "turn1_rad" "turn1_bias" "turn2_dir" "turn2_rad" "turn2_bias" "turn_rad_min" "turn1_rad_mod" "turn2_rad_mod" "turn_rad_mod" "turn1_bias_mod" "turn2_bias_mod" "turn_bias_mod""mid_pct" "leg_spds" "shift_pt" "shift_point" "visual_hints" "turn_pt_gap" "coord" "turn1_ext" "turn2_ext" "turn_ext" "turn1_ext_mod" "turn2_ext_mod" "turn_ext_mod" "adjust_turn" "offboard_tgap" "lane" "id" "coord_extrap" "coord_onleg" )
	 	 
       '("BHV_ZigZag" "speed" "stem_hdg" "stem_on_active" "hdg_thresh" "fierce_zigging" "zig_angle" "zig_first" "max_zig_legs" "max_zig_zags" "visual_hints" "spd_on_active" "speed_on_active" "zigflag" "zagflag" "portflag" "starflag" "portflagx" "starflagx" "zig_angle_fierce" "max_stem_dist" "max_stem_odo" )
	 
       '("BHV_FullStop" "mark_duration" "stop_thresh" "mark_flag" "unmark_flag" "delay_complete" "visual_hints")
	 
       '("BHV_Convoy" "post_mapping" "contact" "radius" "nm_radius" "max_mark_range" "cruise_speed" "spd_max" "spd_slower" "spd_faster" "rng_estop" "range_estop" "rng_tgating" "range_tailgating" "rng_lagging" "range_lagging" "inter_mark_range")

       '("BHV_ConvoyV21" "post_mapping" "contact" "post_recap_verbose" "radius" "capture_radius" "slip_radius" "inter_mark_range" "tail_length_max" "full_stop_convoy_range" "slower_convoy_range" "ideal_convoy_range" "faster_convoy_range" "full_lag_convoy_range" "lag_speed_delta" "convoy_flag" "marker_flag" "visual_hints" "patience" "aft_patience" )

       '("BHV_ConvoyV21X" "post_mapping" "contact" "post_recap_verbose" "radius" "capture_radius" "slip_radius" "inter_mark_range" "tail_length_max" "full_stop_convoy_range" "slower_convoy_range" "ideal_convoy_range" "faster_convoy_range" "full_lag_convoy_range" "lag_speed_delta" "convoy_flag" "marker_flag" "visual_hints" "patience" "aft_patience" "active_convoying")

       '("BHV_Waypoint" "post_mapping" "lead_condition" "capture_radius" "capture_line" "cycleflag" "lead" "lead_damper" "lead_to_start" "order" "points" "point" "polygon" "post_suffix" "radius" "repeat" "nm_radius" "slip_radius" "ipf_type" "ipf-type" "speed" "speed_alt" "use_alt_speed" "visual_hints" "wptflag" "wpt_status_var" "wpt_index_var" "wpt_dist_to_prev" "wpt_dist_to_next" "efficiency_measure" "crs_spd_zaic_ratio" "wptflag_on_start")       

       '("BHV_OpRegionV24" "max_time" "max_depth" "min_altitude" "polygon" "trigger_entry_time" "trigger_exit_time" "visual_hints" "breached_altitude_flag" "breached_depth_flag" "breached_poly_flag" "breached_time_flag" "core_polly" "save_poly" "halt_poly" "draw_halt_status" "draw_save_statue" "save_flag" "savex_flag" "halt_dist" "save_dist")

       '("BHV_OpRegion" "post_mapping" "max_time" "max_depth" "min_altitude" "polygon" "trigger_entry_time" "trigger_exit_time" "visual_hints" "breached_altitude_flag" "breached_depth_flag" "breached_poly_flag" "breached_time_flag" "reset_var" "time_remaining_var" "opregion_poly_var")

       '("BHV_OpRegionRecover" "post_mapping" "polygon" "trigger_entry_time" "trigger_exit_time" "visual_hints" "breached_poly_flag" "reset_var" "opregion_poly_var" "dynamic_region_var" "buffer_dist")
       '("BHV_Loiter" "post_mapping" "acquire_dist" "capture_radius" "center_activate" "center_assign" "clockwise" "polygon" "post_suffix" "radius" "nm_radius" "slip_radius" "speed" "speed_alt" "use_alt_speed" "spiral_factor" "visual_hints" "xcenter_assign" "ycenter_assign" "patience" "ipf-type" "ipf_type")
       '("BHV_PeriodicSpeed" "post_mapping" "basewidth" "initially_busy" "peakwidth" "period_busy" "period_lazy" "period_speed" "reset_upon_running" "summit_delta")
       '("BHV_PeriodicSurface" "post_mapping" "acomms_mark_variable" "ascent_grade" "ascent_speed" "atsurface_status_var" "mark_variable" "max_time_at_surface" "pending_status_var" "period" "zero_speed_depth")
       '("BHV_ConstantDepth" "post_mapping" "basewidth" "depth" "duration" "peakwidth" "summitdelta" "depth_mismatch_var")
       '("BHV_MaxDepth" "post_mapping" "basewidth" "max_depth" "tolerance" "depth_slack_var")
       '("BHV_MaxSpeed" "post_mapping" "basewidth" "max_speed" "tolerance")
       '("BHV_ConstantHeading" "post_mapping" "basewidth" "heading" "duration" "peakwidth" "summitdelta" "heading_mismatch_var" "complete_thresh")
       '("BHV_ConstantSpeed" "post_mapping" "basewidth" "speed" "duration" "peakwidth" "summitdelta" "speed_mismatch_var")
       '("BHV_GoToDepth" "post_mapping" "capture_delta" "capture_flag" "repeat" "perpetual")
       '("BHV_MemoryTurnLimit" "post_mapping" "memory_time" "turn_range")
       '("BHV_StationKeep" "post_mapping" "center_activate" "hibernation_radius" "inner_radius" "outer_radius" "outer_speed" "station_pt" "swing_time" "transit_speed" "visual_hints")
       '("BHV_Timer" "post_mapping" )
       '("BHV_AvoidObstacle" "post_mapping" "poly" "allowable_ttc" "pwt_outer_dist" "pwt_inner_dist" "buffer_dist" "obstacle_key" "completed_dist" "no_alert_request" "i_understand_this_behavior_is_deprecated")
       '("BHV_AvoidObstacleX" "post_mapping" "poly" "allowable_ttc" "pwt_outer_dist" "pwt_inner_dist" "min_util_cpa_dist" "max_util_cpa_dist" "obstacle_key" "completed_dist" "no_alert_request" "use_refinery" "i_understand_this_behavior_is_deprecated")
       '("BHV_AvoidObstacleV21" "post_mapping" "poly" "allowable_ttc" "pwt_outer_dist" "pwt_inner_dist" "min_util_cpa_dist" "max_util_cpa_dist" "obstacle_key" "completed_dist" "no_alert_request" "use_refinery" "id" "visual_hints" )
       '("BHV_AvoidObstacleV24" "post_mapping" "poly" "allowable_ttc" "pwt_outer_dist" "pwt_inner_dist" "min_util_cpa_dist" "max_util_cpa_dist" "obstacle_key" "completed_dist" "no_alert_request" "use_refinery" "id" "visual_hints" )
       '("BHV_HSLine" "post_mapping" "time_on_leg" )
       '("BHV_TestFailure" "post_mapping" "failure_type")
       '("BHV_CutRange" "post_mapping" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "pwt_outer_dist" "pwt_inner_dist" "giveup_dist" "patience" "on_no_contact_ok" "pursueflag" "giveupflag" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region")
       '("BHV_Shadow" "post_mapping" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "pwt_outer_dist" "heading_peakwidth" "heading_basewidth" "speed_peakwidth" "speed_basewidth" "on_no_contact_ok" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region")
       '("BHV_Trail" "post_mapping" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "nm_radius" "n_alert_request" "post_trail_dist_on_idle" "pwt_outer_dist" "radius" "trail_angle" "trail_angle_type" "trail_range" "on_no_contact_ok" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "exit_on_filter_region")

       '("BHV_TaskWaypoint" "post_mapping" "type" "max_bid_wins" "team_range" "team_by_group" "bidwonflag" "xbidwonflag" "waypt_x" "waypt_y" "waypt" )

       '("BHV_TaskMuster" "post_mapping" "type" "max_bid_wins" "team_range" "team_by_group" "bidwonflag" "xbidwonflag" "muster_region" )

       '("BHV_TaskConvoy" "type" "max_bid_wins" "team_range" "team_by_group" "bidwonflag" "xbidwonflag" )

       '("BHV_AndersonTurn" "default_turn_thresh" "default_turn_points" "default_turn_radius" "default_capture_radius" "engage_var" )

       '("BHV_MusterX" "capture_radius" "activate_radius" "speed" "stale_nav_thresh" "muster_region" "setpt_method" "capture_flag" "region_in_flag" "region_out_flag" "visual_hints" "heading_policy" "muster_region_label" "auto_complete" )

       '("BHV_FixedTurn" "fix_turn" "mod_hdg" "turn_dir" "turn_delay" "visual_hints" "speed" "stale_nav_thresh" "turn_spec" "schedule_repeat" )

       '("BHV_AvoidCollision" "post_mapping" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "bearing_lines" "completed_dist" "max_util_cpa_dist" "min_util_cpa_dist" "no_alert_request" "contact_type_required" "on_no_contact_ok" "pwt_grade" "pwt_inner_dist" "pwt_outer_dist" "match_contact_group" "ignore_contact_group" "match_type" "ignore_type" "match_group" "ignore_group" "match_name" "ignore_name" "match_region" "ignore_region" "exit_on_filter_vname" "exit_on_filter_vtype" "exit_on_filter_group" "use_refinery" "bearing_line_label_show" "exit_on_filter_region")

       '("BHV_FixTurn" "fix_turn" "mod_hdg" "turn_dir" "visual_hints" "speed" "stale_nav_thresh" "turn_spec" "schedule" "i_understand_this_behavior_is_deprecated" )
       ))
