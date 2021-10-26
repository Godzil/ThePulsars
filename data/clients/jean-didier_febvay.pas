program finale_prologin_2001;

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

(* Variables globales *)
const Pi = 3.14159265358979323846;

var my_team_id : integer;
    xmax, ymax : single;



type tnature  = (tank, robot);
     t_ami    = record
		   nature : tnature;
		   x, y	  : single;
		   id	  : integer;
		end;
     t_ennemi = record
		   nature : tnature;
		   x, y	  : single;
		   id	  : integer;
		   etat	  : (neutre, disparu, ennemi);
		end;
var amis    : array[1..168+21] of t_ami;
    ennemis : array[1..168*2-21] of t_ennemi;
    nombre_amis,
    nombre_ennemis : integer;

(* Ici nous venons de declarer 2 tableaux : amis et ennemis qui vont stocker toutes les infos possibles concernant les robots et les tanks connus. Ce sera la base de donnees du programme. Ensuite, a partir de ces informations, le programme decidera des actions a effectuer. Les unites ne decident donc pas individuellement des actions a effectuer, sauf dans certains cas particuliers. *)


(* Les variables qui suivent permettent de definir le mouvement d'un tank scrutant ce qu'il y a autour de lui. *)

var rotation, angle, arc_sup, arc_inf	: array[0..168*2-1] of single;
    deja_vu, coeff_x, coeff_y, balayage	: array[0..168*2-1] of boolean;
    sum					: array[0..168*2-1] of integer;

(* Les tableaux suivants permettent de savoir si une unite subit un ordre venant du programme ou est libre de faire ce qu'elle veut' et qui chasse tel ou tel tank ennemi. *)

var occupe   : array[0..168*2-1] of boolean;
    chasseur : array[0..168*2-1] of integer;


(* Le tableau qui suit renvoie l'index d'un ennemi en fonction de son id. *)

var index_ennemis : array[0..168*2-1] of integer;

var concentration : boolean;


procedure player_init(team_id : integer) ;
var i : integer;
begin
   (* On recupere le numero d'id attribue a notre team. *)
   my_team_id := team_id;

   (* Et on enregistre quelques infos utilses dans des variables globales, afin de ne pas avoir a rappeler les fonctions donnant ces infos. *)
   xmax := map_get_size_x;
   ymax := map_get_size_y;

   (* Et on initialise ici quelques variables. *)
   for i := 0 to 168*2-1 do
   begin
      deja_vu[i] := false;
      occupe[i] := false;
      chasseur[i] := -1;
      index_ennemis[i] := 0;
      sum[i] := 0;
      balayage[i] := false;
   end;
   nombre_amis := 0;
   nombre_ennemis := 0;
   concentration := false;
end;

procedure player_new_turn(turn_number : integer) ;
var i, j, k, found : integer;
    d, r : single;
begin
   (* Recherche les ennemis et construit une base de donnees sur eux. *)
   for i := 1 to nombre_amis do if amis[i].nature = tank then
   begin
      (* Recherche les tanks ennemis. *)
      k := map_get_nearest_akx(amis[i].id, -my_team_id);
      if k >= 0 then
      begin
         found := 0;
	 for j := 1 to nombre_ennemis do if ennemis[j].id = k then
	 begin
	    found := 1;
	    with ennemis[j] do
	    begin
	       x := akx_get_pos_x(k);
	       y := akx_get_pos_y(k);
	       if akx_get_team(k) = 0 then etat := neutre
	       else etat := ennemi;
	    end;
	    break;
	 end;
	 if found = 0 then
	 begin
	    inc(nombre_ennemis);
	    index_ennemis[k] := nombre_ennemis;
	    with ennemis[nombre_ennemis] do
	    begin
	       nature := tank;
	       x := akx_get_pos_x(k);
	       y := akx_get_pos_y(k);
	       id := k;
	       if akx_get_team(k) = 0 then etat := neutre
	       else etat := ennemi;
	    end;
	 end;
      end;

      (* Recherche les robots ennemis. *)
      k := map_get_nearest_r4d2(amis[i].id, -my_team_id);
      if k >= 0 then
      begin
         found := 0;
	 for j := 1 to nombre_ennemis do if ennemis[j].id = k then
	 begin
	    found := 1;
	    with ennemis[j] do
	    begin
	       x := r4d2_get_pos_x(k);
	       y := r4d2_get_pos_y(k);
	       if r4d2_get_team(k) = 0 then etat := neutre
	       else etat := ennemi;
	    end;
	    break;
	 end;
	 if found = 0 then
	 begin
	    inc(nombre_ennemis);
	    index_ennemis[k] := nombre_ennemis;
	    with ennemis[nombre_ennemis] do
	    begin
	       nature := robot;
	       x := r4d2_get_pos_x(k);
	       y := r4d2_get_pos_y(k);
	       id := k;
	       if r4d2_get_team(k) = 0 then etat := neutre
	       else etat := ennemi;
	    end;
	 end;
      end;

      (* Si rien de nouveau n'a ete trouve apres un balayage, on diminue le rayon de recherche. C'est une strategie majeure pour mon programme ; en effet, cela me permet d'avoir une assez bonne visibilite. *)
      if (balayage[amis[i].id]) and (sum[amis[i].id] = 0) then
	 angle[amis[i].id] := angle[amis[i].id] / 2;
      if balayage[amis[i].id] then
      begin
	 balayage[amis[i].id] := false;
	 sum[amis[i].id] := 0;
      end;
   end;

   (* Ici, le programme va prendre les decisions en fonction des donnees dont il dispose. Notons que la base de donnees n'est remplie qu'apres la fin du second tour. *)
   if turn_counter > 1 then
   begin

      (* 1) Les robots attaquent les tanks, a raison de 1 robot par tank. *)
      for i := 1 to nombre_ennemis do if (ennemis[i].nature = tank) and (ennemis[i].etat <> disparu) then
      begin
	 (* Verifie si le tank a prendre n'a pas perdu son chasseur s'il en a un. *)
	 if chasseur[ennemis[i].id] > -1 then
	 if (r4d2_get_team(chasseur[ennemis[i].id]) = -1) or
	    (not occupe[chasseur[ennemis[i].id]]) then chasseur[ennemis[i].id] := -1;
      	 (* Recherche du robot le plus pres non affecte a une tache. *)
	 found := 0;
	 r := 1000000;
	 for j := 1 to nombre_amis do
         if (amis[j].nature = robot) and (r4d2_get_status(amis[j].id) = 0) then
	 begin
	    d := sqr(ennemis[i].x-amis[j].x) + sqr(ennemis[i].y-amis[j].y);
	    if d <= 1 then
	    begin
	       occupe[amis[j].id] := false;
	       r4d2_take_akx(amis[j].id, ennemis[i].id);
	       break;
	    end;
	    if (d < r) and (not occupe[amis[j].id]) and (chasseur[ennemis[i].id] = -1) then
	    begin
	       r := d;
	       found := j;
	    end;
	 end;
	 (* S'il existe un tel robot, alors il est envoye vers le tank. *)
	 if found > 0 then
	 begin
	    occupe[amis[found].id] := true;
	    chasseur[ennemis[i].id] := amis[found].id;
	    r4d2_move(amis[found].id, ennemis[i].x, ennemis[i].y);
	 end;
      end;

      (* 2) Tanks. Tirent de maniere concentree sur les R4D2 arrivant. *)
      for i := 1 to nombre_amis do if amis[i].nature = tank then occupe[amis[i].id] := false;
      if not concentration then
      begin
         for i := 1 to nombre_ennemis do if (ennemis[i].nature = robot) and (ennemis[i].etat <> disparu) and (r4d2_get_team(ennemis[i].id) <> -1) then
         begin
	    for j := 1 to nombre_amis do if amis[j].nature = tank then
	    begin
	       akx_pulse(amis[j].id, ennemis[i].x, ennemis[i].y, Pi/25);
	       occupe[amis[j].id] := true;
	    end;
	    concentration := true;
         end;
      end else concentration := false;
   end;

   (* Reinitialise la base de donnees sur les unites amies pour le tour suivant. *)
   nombre_amis := 0;
end;

procedure player_akx_turn(akx_id : integer) ;
var r, x, y : single;
begin
(* Initialisation des parametres de rotation *)
   if not deja_vu[akx_id] then
   begin
      x := akx_get_pos_x(akx_id);
      y := akx_get_pos_y(akx_id);
      angle[akx_id] := Pi/5;
      if (x <= xmax/3) and (y <= ymax/3) then
      begin
         rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi/2;
	 arc_inf[akx_id] := 0;
	 coeff_x[akx_id] := false;
	 coeff_y[akx_id] := false;
      end
      else if (x <= xmax/3) and (y >= 2*ymax/3) then
      begin
         rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi/2;
	 arc_inf[akx_id] := 0;
         coeff_x[akx_id] := false;
	 coeff_y[akx_id] := true;
      end
      else if (x >= 2*xmax/3) and (y <= ymax/3) then
      begin
         rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi/2;
	 arc_inf[akx_id] := 0;
         coeff_x[akx_id] := true;
	 coeff_y[akx_id] := false;
      end
      else if (x >= 2*xmax/3) and (y >= 2*ymax/3) then
      begin
         rotation[akx_id] := Pi;
	 arc_sup[akx_id] := 3*Pi/2;
	 arc_inf[akx_id] := Pi;
         coeff_x[akx_id] := false;
	 coeff_y[akx_id] := false;
      end
      else if y <= ymax/3 then
      begin
	 rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi;
	 arc_inf[akx_id] := 0;
	 coeff_x[akx_id] := false;
	 coeff_y[akx_id] := false;
      end
      else if y >= 2*ymax/3 then
      begin
	 rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi;
	 arc_inf[akx_id] := 0;
	 coeff_x[akx_id] := false;
	 coeff_y[akx_id] := true;
      end
      else if x <= xmax/3 then
      begin
	 rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi/2;
	 arc_inf[akx_id] := -Pi/2;
	 coeff_x[akx_id] := false;
	 coeff_y[akx_id] := false;
      end
      else if x >= 2*xmax/3 then
      begin
	 rotation[akx_id] := 0;
	 arc_sup[akx_id] := Pi/2;
	 arc_inf[akx_id] := -Pi/2;
	 coeff_x[akx_id] := true;
	 coeff_y[akx_id] := false;
      end
      else
      begin
	 rotation[akx_id] := 0;
	 angle[akx_id] := Pi/10;
	 arc_sup[akx_id] := 2*Pi;
	 arc_inf[akx_id] := 0;
         coeff_x[akx_id] := false;
	 coeff_y[akx_id] := false;
      end;
   end;
   deja_vu[akx_id] := true;

   (* Si le tank est un ennemi qui vient d'etre capture par notre team, alors il signale qu'il n'est plus un ennemi. *)
   ennemis[index_ennemis[akx_id]].etat := disparu;
   index_ennemis[akx_id] := 0;

   (* Le tank se declare dans la base de donnees globale. *)
   inc(nombre_amis);
   with amis[nombre_amis] do
   begin
      nature := tank;
      x := akx_get_pos_x(akx_id);
      y := akx_get_pos_y(akx_id);
      id := akx_id;
   end;

   (* Le tank, lorsqu'il n'a aucun ordre de la part du programme, pulse autour de lui, selon certains parametres que le programme principal peut modifier a sa guise. *)
   if not occupe[akx_id] then
   begin
      r := sqrt(2*akx_get_power/(angle[akx_id]*akx_get_see_power)-2/angle[akx_id]) - 0.001;
      x := r * cos(rotation[akx_id]);
      y := r * sin(rotation[akx_id]);
      if coeff_x[akx_id] then x := -x;
      if coeff_y[akx_id] then y := -y;
      akx_pulse(akx_id, amis[nombre_amis].x+x, amis[nombre_amis].y+y, angle[akx_id]);
      rotation[akx_id] := rotation[akx_id] + angle[akx_id];
      if rotation[akx_id] > arc_sup[akx_id] then
      begin
	 rotation[akx_id] := arc_inf[akx_id];
	 balayage[akx_id] := true;
      end;
   end;
end;

procedure player_r4d2_turn(r4d2_id : integer) ;
begin
   (* Si le robot est un ennemi qui vient d'etre capture par notre team, alors il signale qu'il n'est plus un ennemi. *)
   ennemis[index_ennemis[r4d2_id]].etat := disparu;
   index_ennemis[r4d2_id] := 0;

   (* Le robot se declare dans la base de donnees globale. *)
   inc(nombre_amis);
   with amis[nombre_amis] do
   begin
      nature := robot;
      x := r4d2_get_pos_x(r4d2_id);
      y := r4d2_get_pos_y(r4d2_id);
      id := r4d2_id;
   end;
end;

begin
end.