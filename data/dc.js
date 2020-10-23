/*LABELS-DC*/

var labels = new Array();

labels[0] = document.getElementById('label-1');
labels[1] = document.getElementById('label-2');

/*SLIDERS-DC*/

var sliders = new Array();

sliders[0] = document.getElementById('slider-1');
sliders[1] = document.getElementById('slider-2');

sliders[0].oninput = function () { 
    labels[0].innerHTML = 'ШИМ-величина: ' + this.value;
}

sliders[1].oninput = function () {   
    labels[1].innerHTML = 'ШИМ-величина: ' + this.value;
}

sliders[0].onchange = function () {
    var xhttp = new XMLHttpRequest();       
    xhttp.open('GET', '/dc?dc_pwm_1=' + this.value, true);
    xhttp.send();
}

sliders[1].onchange = function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open('GET', '/dc?dc_pwm_2=' + this.value, true);
    xhttp.send();
}

/*SWITCHES-DC*/

var switches = new Array();

switches[0] = document.getElementById('switch-1');
switches[1] = document.getElementById('switch-2');

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[0].checked) {
        xhttp.open('GET', '/dc?dc_state_1=ON', true);
    }
    else {
        xhttp.open('GET', '/dc?dc_state_1=OFF', true);
    }
    xhttp.send();
};

switches[1].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[1].checked) {
        xhttp.open('GET', '/dc?dc_state_2=ON', true);
    }
    else {
        xhttp.open('GET', '/dc?dc_state_2=OFF', true);
    }
    xhttp.send();
};

/*INI-DC*/

var DC_PWM = new Array();
var DC_STATE = new Array();

function dc_ini () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {        
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);            
            for (let i = 0; i < data.length; i++) {
                sliders[i].value = data[i].dc_pwm;
                if (data[i].dc_state == 'ON') {
                    switches[i].checked = true;
                }
                else {
                    switches[i].checked = false;
                }                
                labels[i].innerHTML = 'ШИМ-величина: ' + sliders[i].value;
            }          
        }
    };
    xhttp.open('GET', '/dc_ini', true);
    xhttp.send();
}

dc_ini();