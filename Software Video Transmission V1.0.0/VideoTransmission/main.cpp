#include "mainwindow.h"
#include <QApplication>
/*
 * Funcionamiento General:
 * 1_ El programa inicia minimizado.
 * 2_ Se carga la configuracion por defecto.
 * 3_ Se crean los distintos objetos: dispositivo, TCP, UDP, SHM,
 * 4_ Segun la configuracion cargada y segun se hayan Configurado las pestañas "Device Config." y "Communication Config."
 * Se disparan (o detienen) los hilos de comunicacion (TCP/UDP/SHM)
 *   Ademas se disparan 2 timmers aproximadamente a 20 hz, utilizados para externalizar los datos.
 * Uno para las configuraciones de entrada y otro para los datos de salida.
 * 5_Al darle "Start" se dispara un evento que señaliza al dispositivo para arrancar ("start_device"):
 * internamente dispositivo maneja su sistema hilado (crea/cierra comunicacion e ingresa en el lazo periodico)
 * con las configuraciones cargadas en la ventana principal.
 * 6_ Mientras el lazo corre puede o no modificarse las configuraciones, tanto del dispositivo como de la comunicacion.
 * aqui los parametros que pueden modificarse quedan habilitados y el resto son deshabilitados hasta recibir la señal "stop_device"

 * Notas de funcionamiento:
 * -Las configuraciones del dispositivo deben estar señalizadas por pulso(valor ALTO) para el caso de provenir del TCP,UDP,SHM
 * este pulso debe disparar un evento "new_device_config" que se atrapa en el  hilo principal y se actualizan las casillas de la ventana.
 * Las configuraciones de la ventana NO se actualizan, sino que se muestran las entrantes en la pestaña "device input".
 * En la configuracion debe indicarse de donde proviene (TCP,UDP,SHM) siendo la prioridad 1_SHM 2_TCP 3_UDP.
 * -El objeto dispositivo debe heredad de la clase del hardware usado (eje una camara). Dispositivo debe manejar todo el comportamiento hilado
 * y metodos de comunicacion. Debe tener 3 secciones basicas: config (recepciona y prioriza las configuraciones), connect (inicializa el dispositivo), run(bucle principal)
 * stop (detiene el dispositivo), ademas del constructor y destructor.
 * */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(0,a.applicationPid());
    w.show();
    return a.exec();
}
