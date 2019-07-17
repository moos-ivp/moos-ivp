
test_name = 01

start_time = 20
end_time   = 360

//start_condition = DEPLOY = true

pass_condition = IVPHELM_STATE=DRIVE
pass_condition = NAV_X > -200
//pass_condition = MISSION = complete
fail_condition = COLLISION = true

//--------------------------------------------
test_name = 02

start_time = 30
end_time   = 90

start_condition = DEPLOY = yes
end_condition   = DEPLOY = now

pass_condition = MISSION = done
fail_condition = COLLISION = yes

