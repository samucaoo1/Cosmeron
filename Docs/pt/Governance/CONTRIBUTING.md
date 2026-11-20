# Como contribuir com a Cosmeron

Contribuições são bem-vindas quando preservam os objetivos centrais da Cosmeron: C legível, comportamento explícito, arquitetura modular, portabilidade e código que continue útil para estudo.

Antes de alterar APIs públicas, leia o [Padrão Cosmeron](../Reference/PATTERN.md).

## Regras básicas

A Cosmeron tem como alvo o **C11**. APIs públicas devem permanecer portáveis e evitar expor terminologia específica de sistemas operacionais, a menos que a API seja explicitamente voltada a recursos nativos.

Siga as convenções estabelecidas de nomenclatura, namespaces, propriedade, status e parâmetros de saída. Prefira os auxiliares de pré-processador existentes em vez de recriar concatenação de tokens ou mecanismos de namespace por conta própria.

Sempre que viável, preserve a filosofia **zero-link**: o usuário não deve precisar adicionar uma nova opção obrigatória ao linker quando um recurso puder ser implementado sem ela.

## Alterações

Mantenha as alterações focadas. Se uma refatoração modificar um padrão estabelecido em vários módulos, documente primeiro a regra ou atualize o Padrão na mesma alteração.

Novos comportamentos públicos devem incluir:

- testes que cubram sucesso, limites e erros;
- documentação da API pública;
- exemplos quando o uso pretendido não for evidente;
- consideração de GCC, Clang, MinGW, MSVC e Clang no macOS, quando aplicável.

## Testes

O projeto usa a compilação estrita em C11 como referência:

```text
-std=c11 -Wall -Wextra -Wpedantic -Werror
```

Os testes dos módulos devem permanecer organizados por módulo e exercitar todas as funções públicas afetadas pela alteração.

Não reduza o nível de avisos apenas para fazer uma alteração passar.

## Pull requests

Um pull request deve explicar **o que mudou**, **por que a mudança pertence à Cosmeron** e quais são as consequências para portabilidade ou compatibilidade.

Pull requests pequenos e coerentes são mais fáceis de revisar do que alterações sem relação entre si agrupadas.

## Documentação

A documentação canônica do projeto fica em `Docs/`.

- governança: `Docs/Governance/`
- direção do projeto e versões: `Docs/Project/`
- regras técnicas transversais: `Docs/Reference/`
- documentação dos módulos: `Docs/Modules/`
- traduções: `Docs/pt/` e `Docs/es/`

Mantenha a raiz do repositório reservada a arquivos com função no nível do repositório ou de descoberta por ferramentas.

## Segurança

Não abra issues públicas para vulnerabilidades que exijam detalhes confidenciais. Siga a [Política de Segurança](SECURITY.md) e utilize o recurso de comunicação privada de vulnerabilidades do GitHub.

## Conduta

A participação é regida pelo [Código de Conduta](CODE_OF_CONDUCT.md).
