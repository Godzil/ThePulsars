function turn_number : integer; external; c ;
function turn_counter : integer; external; c ;

function time_get_left : integer; external ; c ;

function score_get : integer; external ; c ;

function error_get : integer; external ; c ;

function r4d2_get_team(r4d2_id : integer) : integer; external ; c ;
function r4d2_get_pos_x(r4d2_id : integer) : single; external ; c ;
function r4d2_get_pos_y(r4d2_id : integer) : single; external ; c ;
function r4d2_get_status(r4d2_id : integer) : integer; external ; c ;

function r4d2_get_speed : single; external; c ;
function r4d2_get_destroy_speed : single; external; c ;

function r4d2_turn_take_r4d2 : integer; external; c ;
function r4d2_turn_untake_r4d2 : integer; external; c ;
function r4d2_turn_take_akx : integer; external; c ;
function r4d2_turn_untake_akx : integer ; external; c ;

function r4d2_move(r4d2_id : integer; dx, dy : single) : boolean; external ; c ;
function r4d2_take_akx(r4d2_id, target_id : integer) : boolean; external ; c ;
function r4d2_take_r4d2(r4d2_id, target_id : integer) : boolean; external ; c ;

function akx_get_team(r4d2_id : integer) : integer; external ; c ;
function akx_get_pos_x(r4d2_id : integer) : single; external ; c ;
function akx_get_pos_y(r4d2_id : integer) : single; external ; c ;
function akx_get_status(r4d2_id : integer) : integer; external ; c ;

function akx_pulse_angle(r4d2_id : integer) : single; external ; c ;
function akx_pulse_destx(r4d2_id : integer) : single; external ; c ;
function akx_pulse_desty(r4d2_id : integer) : single; external ; c ;

function akx_get_speed : single; external ; c ;
function akx_get_power : single; external ; c ;
function akx_get_pulse_coef : single; external ; c ;
   function akx_get_see_power : single; external ; c ;

function akx_move(r4d2_id : integer; dx, dy : single) : boolean; external ; c ;
function akx_pulse(r4d2_id : integer; dx, dy, angle : single) : boolean; external ; c ;
function akx_link(r4d2_id, target_id : integer) : boolean; external ; c ;

function map_get_size_x : single; external ; c ;
function map_get_size_y : single; external; c ;
function map_get_pulse(team_id : integer; x, y : single) : single; external ; c ;
function map_get_pulse_id(akx_id : integer; x, y : single) : single; external ; c ;
function map_count_akx : integer; external ; c ;
function map_count_r4d2 : integer; external ; c ;
function map_count_my_akx : integer; external ; c ;
function map_count_my_r4d2 : integer; external ; c ;
function map_get_nearest_akx_plot(x, y : single; team_id : integer) : integer; external ; c ;
function map_get_nearest_r4d2_plot(x, y : single ; team_id : integer) : integer; external ; c ;
function map_get_nearest_akx(id, team_id : integer) : integer; external ; c ;
function map_get_nearest_r4d2(id, team_id	: integer ): integer; external ; c ;

			   (* Vous devez coder: *)

procedure player_init(team_id : integer) ;
begin
end;

procedure player_new_turn(turn_number : integer) ;
begin
end;

procedure player_akx_turn(akx_id : integer) ;
begin
end;

procedure player_r4d2_turn(r4d2_id : integer) ;
begin
end;

begin
end.
