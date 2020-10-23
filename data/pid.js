/*LABELS-PID*/

var labels = new Array();

labels[0] = document.getElementById('label-1');
labels[1] = document.getElementById('label-2');
labels[2] = document.getElementById('label-3');

/*SLIDERS-PID*/

var sliders = new Array();

sliders[0] = document.getElementById('slider-1');
sliders[1] = document.getElementById('slider-2');
sliders[2] = document.getElementById('slider-3');

sliders[0].oninput = function () {
    labels[0].innerHTML = 'Kp: ' + this.value;
}

sliders[1].oninput = function () {
    labels[1].innerHTML = 'Ki: ' + this.value;
}

sliders[2].oninput = function () {
    labels[2].innerHTML = 'Kd: ' + this.value;
}

/*INPUT-PID*/

var input = document.getElementById('input');

/*SWITCHES-PID*/

var switches = new Array();

switches[0] = document.getElementById('switch-1');
switches[1] = document.getElementById('switch-2');

switches[0].onchange = function () {
    var xhttp = new XMLHttpRequest();
    if (!switches[0].checked) {   
        sliders[0].disabled = true;
        sliders[1].disabled = true;
        sliders[2].disabled = true;
        input.disabled = true;
        $('.slider').toggleClass('slider-disable');
        $('.input').toggleClass('input-disable');     
        setpoint = input.value;
        xhttp.open('GET', '/pid?kp=' + sliders[0].value + 
            '&ki=' + sliders[1].value + '&kd=' + sliders[2].value + '&setpoint=' + input.value, true);
        xhttp.send();
    }
    else {
        sliders[0].disabled = false;
        sliders[1].disabled = false;
        sliders[2].disabled = false;
        input.disabled = false;
        $('.slider').toggleClass('slider-disable', false);
        $('.input').toggleClass('input-disable', false);
    }
};

switches[1].onchange = function () {
    var xhttp = new XMLHttpRequest();    
    if (switches[1].checked) {
        xhttp.open('GET', '/pid?pid_state=ON', true);
    }
    else {
        xhttp.open('GET', '/pid?pid_state=OFF', true);
    }
    xhttp.send();
};

/*INI-PID*/

function pid_ini () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var data = this.responseText;
            data = data.replace(/'/g, '"');            
            data = JSON.parse(data);
            sliders[0].value = data[0].kp;
            sliders[1].value = data[0].ki;       
            sliders[2].value = data[0].kd;   
            input.value = data[0].setpoint;
            labels[0].innerHTML = 'Kp: ' + sliders[0].value;
            labels[1].innerHTML = 'Ki: ' + sliders[1].value;
            labels[2].innerHTML = 'Kd: ' + sliders[2].value;
            if (data[0].pid_state == 'ON') {
                switches[1].checked = true;
            }
            else {
                switches[1].checked = false;
            }                
        }
    };
    xhttp.open('GET', '/pid_ini', true);
    xhttp.send();
    sliders[0].disabled = true;
    sliders[1].disabled = true;
    sliders[2].disabled = true;
    input.disabled = true;
    switches[0].checked = false;
    $('.slider').toggleClass('slider-disable');
    $('.input').toggleClass('input-disable');
}

pid_ini();