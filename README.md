# TCC ELETRONICA 2021

Escola: ETEC Philadelpho Gouvêa Netto

Orientador: Sérgio Tadao Cosequi

Grupo: Amanda Rodrigues Pereira, Cauan Felipe Tavares, Diego Cauche dos Santos, Gabriel Paschoalão Amaral e Geovana Correa de Souza



## Projeto SAM - Sistema de Auxílio ao uso de Medicamentos


### Motivação

  Este foi um projeto idealiado para ajudar pessoas que se esquecem de tomar remédios, feito com o foco em idosos, que geralmente tomam muitos remédios e comumente esquecem de tomar seus medicamentos.
  
### Interface

  Ultilizamos a plataforma Adafruit.io para fazer a interface, onde pelo protocolo MQTT as informações da interface eram enviadas ao esp, fazendo assim o controle do projeto remotamente.
  
Plataforma: https://io.adafruit.com/diegosantos/dashboards/tcc

### Desenvolvimento

  Usamos o protocolo NTP para a recepção da hora em tempo real para executarmos a liberação no tempo correto. Isso possibilitou comparar a hora exata com o horário configurado para ser liberado o remédio. Para liberar o remédio usamos um servo motor que puxava uma caixinha e fazia o remédio escorregar por um cano até um baú externo para recepção.
