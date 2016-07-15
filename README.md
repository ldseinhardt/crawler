# Crawler - Trabalho de Redes - UFPEL

#### Nome: Luan Einhardt

#### Relatórios

#### Trabalho 1

A implementação do trabalho foi feita em C++, foi pensado em uma classe para o cliente HTTP que tem como implementado a função de conexão, uma função para requisições do tipo GET e algumas funções auxiliares para salvar arquivos, pegar a resposta, ...; Uma classe URL que dado um link quebra esse link em protocolo, host, pasta, ...; e uma classe Crawler que inicializa o crawler indo à primeira página e baixando os dados, e a partir dessa indo até o nível requisitado (criando até 7 threads no máximo, pois o trabalho tem como requisitos no máximo 8 threads em paralelo, e o programa já tem uma thread como thread principal).

Foram realizados testes até o nível 2 em sites como www.google.com.br, portal.ufpel.edu.br, www.baixaki.com.br (baixa várias imagens <img src="" /> em relação aos outros testados)

#### Trabalho 2

Realizado testes com entrada http e https de profundidade 2 em www.google.com.br, certificado válido e tudo ok, baixado mais de 70MB em arquivos;

Realizado testes com entrada https em de profundidade 2 www.pcwebshop.co.uk, certificado autoassinado, destacado na cor amarela, arquivos html e imagens baixadas.

Realizado testes com entrada https de profundidade 2 em www.ufpel.edu.br, certificado válido em nome de UNIVERSIDADE FEDERAL DE PELOTAS.

Em alguns casos há uma espera, pelo fato de não poder haver mais que 7 threads baixando páginas.
