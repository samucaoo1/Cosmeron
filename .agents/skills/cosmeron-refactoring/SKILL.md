---
name: cosmeron-refactoring
description: >-
  Diretrizes e procedimentos para a refatoração completa do ecossistema Cosmeron.
  Garante adoção rigorosa dos novos padrões arquiteturais sem criar camadas de
  retrocompatibilidade ou manter código obsoleto.
---

# Cosmeron Refactoring Skill

Esta skill rege a refatoração total da base de código do **Cosmeron**.

> ⚠️ **REGRA FUNDAMENTAL: ZERO RETROCOMPATIBILIDADE**  
> **Não preserve nem crie camadas, aliases, wrappers ou macros de compatibilidade antiga.**  
> O projeto está passando por uma modernização completa de padrões. Qualquer construção legada encontrada deve ser **removida e substituída diretamente** pela sua forma canônica atualizada.

---

## 1. Padrões Canônicos a Adotar

Baseie toda refatoração nas regras de [AGENTS.md](file:///run/media/okarin/HD/Programação/Cosmeron/AGENTS.md) e [PATTERN.md](file:///run/media/okarin/HD/Programação/Cosmeron/Docs/Reference/PATTERN.md).

### A. Callbacks de Primeira Classe (`Core/Module/Callback/Callback.h`)
- **Proibido:** Ponteiros de função brutos para callbacks e separação entre versão comum e `*WithContext`.
- **Obrigatório:** Declarar tipos de callback usando `CALLBACK_DECLARE*`:
  - `CALLBACK_DECLARE(Name, ReturnType, Parameters, Arguments)`
  - `CALLBACK_DECLARE_PROCEDURE(Name, Parameters, Arguments)`
  - `CALLBACK_DECLARE_OPERATION(Name, Parameters, Arguments)`
  - `CALLBACK_DECLARE_PREDICATE(Name, Parameters, Arguments)`
  - Ou suas variantes `NULLARY`.
- **Tipo de uso:** `TCALLBACK(Name)`.
- **Invocação:** `CALLBACK_FUNC(Name_Invoke)(callback, args...)`.
- **Exemplo Comparador:** `TCALLBACK(Comparison)`. Funções de ordenação ou busca recebem unicamente `TCALLBACK(Comparison)`, eliminando duplicatas `WithContext`.

### B. Módulos Declarativos: `PACKAGE` vs `OBJECT`
- **Módulos estáticos / funções livres:** Use `PACKAGE(Namespace, ...)`.
- **Estruturas com estado / instâncias:** Use `OBJECT(Type, FIELDS(...), ...)`.
- **Contratos explícitos de retorno (Substitutos de `METHOD`):**
  - `OPERATION(Name, (params))` $\to$ retorna `OPSTATUS`.
  - `PREDICATE(Name, (params))` $\to$ retorna `bool`.
  - `FUNCTION(ReturnType, Name, (params))` $\to$ retorna valor do tipo `ReturnType`.
  - `PROCEDURE(Name, (params))` $\to$ ação infalível retornando `void`.
  - Para métodos em objetos que não modificam estado: use prefixo `CONST_*`.
- **Proibido em código novo/refatorado:**
  - Macros obsoletas: `METHOD`, `CONST_METHOD`, `OBJECT_PROTOTYPE`, `PACKAGE_PROTOTYPE`, `OBJECT_PROTOTYPEEX`, `PACKAGE_PROTOTYPEEX`.
  - Termos como `VTable` (sempre use `FunctionTable`).

### C. Implementações em `.impl`
- Prefira a família explícita de macros de implementação:
  ```c
  /* Para Objetos (amarra Target *self como primeiro parâmetro) */
  OBJECT_OPERATION_IMPLEMENT_EX(Target, MethodName, (params...)) { ... }
  OBJECT_PREDICATE_IMPLEMENT_EX(Target, MethodName, (params...)) { ... }
  OBJECT_FUNCTION_IMPLEMENT_EX(Target, ReturnType, MethodName, (params...)) { ... }
  OBJECT_PROCEDURE_IMPLEMENT_EX(Target, MethodName, (params...)) { ... }

  /* Para Pacotes (funções livres) */
  PACKAGE_OPERATION_IMPLEMENT_EX(Package, FuncName, (params...)) { ... }
  PACKAGE_PREDICATE_IMPLEMENT_EX(Package, FuncName, (params...)) { ... }
  PACKAGE_FUNCTION_IMPLEMENT_EX(Package, ReturnType, FuncName, (params...)) { ... }
  PACKAGE_PROCEDURE_IMPLEMENT_EX(Package, FuncName, (params...)) { ... }
  ```
- Finalize sempre o arquivo `.impl` com:
  - `PACKAGE_IMPLEMENT(PackageName);` ou
  - `OBJECT_IMPLEMENT(ObjectName);`

### D. Enums e Tabelas X-Macro
- **Proibido:** Enums literais manuais `enum { FOO, BAR }`.
- **Obrigatório:** Toda enumeração deve possuir sua tabela X-Macro (`<DOMAIN>_TABLE(X)`).
- Prefixos de constante pública: `*_CONST(NAME)`.

### E. Convenção de Tipos e Nomes
- Prefixo `T` **apenas** para tipos diretamente manipulados pelo usuário (`TMemoryPool`, `TVector_uint32`, `TCALLBACK(...)`).
- Enums de saída não levam `T` (ex: `ComparisonResult`, aliased para `CMPOUT`).
- Nomes qualificados evitam colisões (`TMemoryPool` em vez de `TPool`).

### F. Motor Genérico de Templates (`Core/Module/Common/Template.impl`)
- **Proibido:** Criar macros monolíticas ad-hoc de instanciação (ex: `VECTOR_IMPLEMENT_ALL`, `*_IMPLEMENT_ALL_DEGREE_CMP`).
- **Obrigatório:** Seguir o padrão de templates do Cosmeron:
  1. `TEMPLATE_<NAME>(SUFFIX, TYPE)`: declaração do `OBJECT` ou `PACKAGE`.
  2. `TEMPLATE_<NAME>_IMPLEMENT(SUFFIX, TYPE)`: implementação dos métodos via `*_IMPLEMENT_EX`.
  3. Para templates puramente inline: definir `#define TEMPLATE_<NAME>_INLINE_ONLY 1` ou `#define TEMPLATE_<NAME>_IMPLEMENT(SUFFIX, TYPE) /* vazio */`.
- **Instanciação Padronizada:**
  - `TEMPLATE_DECLARE(NAME, SUFFIX, TYPE)`: instanciação somente de tipos e protótipos em headers (`.h`).
  - `TEMPLATE_IMPLEMENT(NAME, SUFFIX, TYPE)`: instanciação completa (declaração + corpos) em arquivos de implementação (`.impl` / `.c`).
- **Nomenclatura gerada:** Sempre usar `T<Name>_<suffix>` para tipos de usuário (ex: `TStack_int`, `TVector_uint32`).

---

## 2. Checklist do Procedimento de Refatoração

Ao refatorar um módulo listado em `.temp`:

1. **Inspecione o cabeçalho (`.h`) e a implementação (`.impl`)**:
   - Identifique assinaturas antigas (`*_PROTOTYPE`, `METHOD`, ponteiros brutos de callback, etc.).
2. **Elimine duplicações de contexto**:
   - Onde houver `Func` e `FuncWithContext`, unifique em uma única função recebendo o respectivo `TCALLBACK(...)`.
3. **Substitua por `PACKAGE` ou `OBJECT`**:
   - Reescreva os membros usando `OPERATION`, `PREDICATE`, `FUNCTION` e `PROCEDURE`.
4. **Remova shims e compatibilidade**:
   - Delete os `#define` de compatibilidade legada.
   - Delete typedefs obsoletos.
5. **Atualize o arquivo de implementação (`.impl`)**:
   - Converta para `PACKAGE_*_IMPLEMENT_EX` ou `OBJECT_*_IMPLEMENT_EX`.
   - Remova wrappers intermediários de chamada.
6. **Atualize os testes correspondentes**:
   - Adapte as chamadas no diretório `Codespace/Tests/` para o novo padrão.
7. **Marque o item como concluído no `.temp`**.
