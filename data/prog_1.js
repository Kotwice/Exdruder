var header = [
    ['Материал'], ['$T_{пл} [C^{\circ}]$'],
    ['$T_{экс} [C^{\circ}]$'], ['$T_{стек} [C^{\circ}]$'], 
    ['$T_{карам} [C^{\circ}]$'], ['$T_{крит} [C^{\circ}]$'],
    ['$ПТР [г/10 мин]$'], ['$\rho [г/см^3]$'],
    ['$T_{разгм} [C^{\circ}]$']
];

var cells = [
    ['PLA'], ['145-180'], ['175-230'], ['60'], ['245'], ['260'], ['6.09'], ['1.24'], ['50']
];

var light_color = '#ffffff';
var dark_color = '#272727';
var font_name = 'Times New Roman';
var size_font = 11;

var data = [{
    type: 'table',
    header: {
        values: header,
        align: 'center',
        line: {width: 1, color: light_color},
        fill: {color: dark_color},
        font: {family: font_name, size: size_font, color: light_color}
    },
    cells: {
        values: cells,
        align: 'center',
        line: {color: light_color, width: 1},
        fill: {color: dark_color},
        font: {family: font_name, size: size_font, color: [light_color]}
    }
}]

Plotly.newPlot('prog-1', data);