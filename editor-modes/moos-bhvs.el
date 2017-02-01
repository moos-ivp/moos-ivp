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
       '("BHV_Waypoint" "capture_radius" "capture_line" "cycleflag" "lead" "lead_damper" "lead_to_start" "order" "points" "point" "polygon" "post_suffix" "radius" "repeat" "nm_radius" "slip_radius" "ipf_type" "ipf-type" "speed" "speed_alt" "use_alt_speed" "visual_hints" "wptflag" "wpt_status_var" "wpt_index_var" "wpt_dist_to_prev" "wpt_dist_to_next" "efficiency_measure" "crs_spd_zaic_ratio")
       '("BHV_OpRegion" "max_time" "max_depth" "min_altitude" "polygon" "trigger_entry_time" "trigger_exit_time" "visual_hints" "breached_altitude_flag" "breached_depth_flag" "breached_poly_flag" "breached_time_flag" "reset_var" "time_remaining_var" "opregion_poly_var")
       '("BHV_OpRegionRecover" "polygon" "trigger_entry_time" "trigger_exit_time" "visual_hints" "breached_poly_flag" "reset_var" "opregion_poly_var")
       '("BHV_Loiter" "acquire_dist" "capture_radius" "center_activate" "center_assign" "clockwise" "polygon" "post_suffix" "radius" "nm_radius" "slip_radius" "speed" "speed_alt" "use_alt_speed" "spiral_factor" "visual_hints" "xcenter_assigne" "ycenter_assign" "patience")
       '("BHV_PeriodicSpeed" "basewidth" "initially_busy" "peakwidth" "period_busy" "period_lazy" "period_speed" "reset_upon_running" "summit_delta")
       '("BHV_PeriodicSurface" "acomms_mark_variable" "ascent_grade" "ascent_speed" "atsurface_status_var" "mark_variable" "max_time_at_surface" "pending_status_var" "period" "zero_speed_depth")
       '("BHV_ConstantDepth" "basewidth" "depth" "duration" "peakwidth" "summitdelta" "depth_mismatch_var")
       '("BHV_MaxDepth" "basewidth" "max_depth" "tolerance" "depth_slack_var")
       '("BHV_ConstantHeading" "basewidth" "heading" "duration" "peakwidth" "summitdelta" "heading_mismatch_var" "complete_thresh")
       '("BHV_ConstantSpeed" "basewidth" "speed" "duration" "peakwidth" "summitdelta" "speed_mismatch_var")
       '("BHV_GoToDepth" "capture_delta" "capture_flag" "repeat" "perpetual")
       '("BHV_MemoryTurnLimit" "memory_time" "turn_range")
       '("BHV_StationKeep" "center_activate" "hibernation_radius" "inner_radius" "outer_radius" "outer_speed" "station_pt" "swing_time" "transit_speed" "visual_hints")
       '("BHV_Timer")
       '("BHV_AvoidObstacles" "poly" "allowable_ttc" "activation_dist" "buffer_dist" "pwt_outer_dist" "pwt_inner_dist" "obstacle_key" "completed_dist" "visual_hints")
       '("BHV_AvoidObstacle" "poly" "allowable_ttc" "pwt_outer_dist" "pwt_inner_dist" "buffer_dist" "obstacle_key" "completed_dist" "no_alert_request")
       '("BHV_HSLine" "time_on_leg")
       '("BHV_TestFailure" "failure_type")
       '("BHV_CutRange" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "pwt_outer_dist" "pwt_inner_dist" "giveup_dist" "patience" "on_no_contact_ok")
       '("BHV_Shadow" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "pwt_outer_dist" "heading_peakwidth" "heading_basewidth" "speed_peakwidth" "speed_basewidth" "on_no_contact_ok")
       '("BHV_Trail" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "nm_radius" "n_alert_request" "post_trail_dist_on_idle" "pwt_outer_dist" "radius" "trail_angle" "trail_angle_type" "trail_range" "on_no_contact_ok")
       '("BHV_AvoidCollision" "contact" "them" "extrapolate" "decay" "decay_end" "time_on_leg" "bearing_line_config" "bearing_lines" "completed_dist" "max_util_cpa_dist" "min_util_cpa_dist" "no_alert_request" "contact_type_required" "on_no_contact_ok" "pwt_grade" "pwt_inner_dist" "pwt_outer_dist" "match_contact_group" "ignore_contact_group")))
