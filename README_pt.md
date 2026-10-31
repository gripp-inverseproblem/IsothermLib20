# Isotherm++ 2.0, correção incremental da migração Smith

**Base auditada:** `Isotherm(3).zip`, recebido em 30/10/2026.
**Conteúdo:** patch unificado, relatório técnico, matrizes de validação.
**Escopo:** somente a isoterma Smith e seus pontos de integração.

## Equação e interface

A formulação preservada é `q_e(T; A,B,C,a_w) = A + B*T - C*ln(1-a_w)`.
Os quatro valores armazenados são `A, B, C, a_w`; apenas `A,B,C`
são coeficientes empíricos. `a_w` é uma condição física armazenada. `T` é a
variável independente, sempre em kelvin. A derivada genérica é `dq_e/dT = B`.

```cpp
const isotherm::models::Smith model{1.4, -0.001, 0.55, 0.5};
const isotherm::Real q = isotherm::evaluate(model, 300.0);
const isotherm::Real dq_dT = isotherm::derivative(model, 300.0);
```

## Aplicação segura

1. Abra um terminal na raiz do repositório atual, após salvar seu trabalho.
2. Extraia este ZIP em uma pasta separada, fora da árvore do projeto.
3. Verifique o estado do repositório: `git status --short`.
4. Pré-valide as mudanças, sem gravar nada:

```bash
cd ~/Programas/Isotherm
git apply --check /caminho/extraido/Smith_FourParameters_Migration_2026-10-30.patch
```

5. Somente se a verificação passar, aplique o patch:

```bash
git apply /caminho/extraido/Smith_FourParameters_Migration_2026-10-30.patch
git diff --check
git status --short
```

6. Reconfigure os targets CMake (necessário porque os diretórios dos programas
   mudam), compile e execute os testes:

```bash
cmake -S . -B build
cmake --build build --target example_FourParameters_Smith -j 2
cmake --build build --target gtest_FourParameters_Smith -j 2
ctest --test-dir build -R Smith --output-on-failure
cmake --build build --target docs-html -j 2
ctest --test-dir build --output-on-failure
```

O target GoogleTest requer que a dependência GTest tenha sido encontrada na
configuração. Se o build atual utiliza presets ou opções, preserve as opções
anteriores ao reconfigurar; não substitua arbitrariamente suas flags.

## Arquivos e comportamento

- Cabeçalho Smith: de `ThreeParameters/Smith.hpp` para
  `FourParameters/Smith.hpp`. Parâmetros e trait atualizados.
- Testes e exemplos: movidos de `ThreeParameters/Smith/` para
  `FourParameters/Smith/`, com assinatura escalar e novos testes.
- Documentação: `Smith.rst` realocado, texto e ligações ao navegador
  atualizados; índice agrupado pelo número de **valores armazenados**.
- CMake de figuras: depende agora de `example_FourParameters_Smith`.
- O gerador C++ emite 81 temperaturas (273.15 a 323.15 K), com 4
  sensibilidades ponderadas; `plot.py` valida 7 colunas e gera 3 SVG.

**Atenção:** as três figuras `smith_curve.svg`, `smith_derivative.svg` e
`smith_sensitivity.svg` passam a mostrar a variação com **temperatura**, em
atividade fixa, conforme a convenção Henderson. A derivada analítica é
uma reta horizontal em `B`. Os arquivos SVG antigos precisam ser
regenerados pelo target de documentação, não apenas mantidos do ZIP.
Os perfis de atividade anteriores podem ser recuperados futuramente
construindo uma sequência de instâncias Smith em diferentes `a_w`.

## Validações executadas fora do ambiente do usuário

- CMake configurou e compilou `example_FourParameters_Smith` com g++20.
- `plot.py` executou e gerou o CSV e três SVG com matplotlib.
- Navegador de código gerou 177 páginas originais, 22 símbolos indexados.
- Comparação C++ antigo vs novo: 243 combinações (3 temperaturas x 81
  atividades), maior diferença absoluta em q_e e dq/da_w = 0.
- 6 blocos de testes da nova suíte foram executados com um harness de
  asserções local; **não foi execução com o GoogleTest real**.
- `git diff --check` e `git apply --check` passaram na cópia do ZIP.

Não foram executados: GTest instalado pelo projeto, CTest global, build
Sphinx com warnings como erros e teste de instalação do pacote.
O ZIP enviado não inclui uma pasta `tools/`, apesar de o CMakeLists.txt
fazer referência a arquivos em `tools/`. Esse problema estrutural preexistente
precisa ser verificado na árvore real antes de atribuir falhas ao Smith.

**Se o `git apply --check` falhar, não force a aplicação**. Isso significa
que a árvore local difere do snapshot usado na preparação; deve-se analisar
o diff antes de escolher o próximo passo.
