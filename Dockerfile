# 1. Imagem Base
# Usamos uma versão específica do Ubuntu para garantir a reprodutibilidade.
FROM ubuntu:22.04

# Evita que a instalação de pacotes peça entradas interativas.
ENV DEBIAN_FRONTEND=noninteractive

# 2. Instalação de Dependências Essenciais
# Instala todas as ferramentas necessárias para compilar o projeto Pico.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    build-essential \
    python3 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# 3. Clonagem do SDK do Raspberry Pi Pico
# Clona o SDK e seus submódulos para um local padrão dentro do contêiner.
RUN git clone --depth 1 https://github.com/raspberrypi/pico-sdk.git /pico/pico-sdk && \
    cd /pico/pico-sdk && \
    git submodule update --init

# 4. Configuração de Variáveis de Ambiente
# Define a variável PICO_SDK_PATH para que o CMake encontre o SDK automaticamente.
# Isso resolve a maioria dos erros de "pico_sdk_import.cmake not found".
ENV PICO_SDK_PATH=/pico/pico-sdk

# 5. Criação de um Usuário Não-Root (Boa Prática de Segurança)
# Evita problemas de permissão de arquivos entre o contêiner e sua máquina.
RUN useradd -ms /bin/bash vscode
USER vscode

# 6. Definição do Diretório de Trabalho
WORKDIR /workspaces