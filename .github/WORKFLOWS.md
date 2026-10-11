# Cosmeron GitHub Actions

Quatro workflows aparecem na aba **Actions**, cada um com uma responsabilidade:

| Workflow | Finalidade | Quando roda |
| --- | --- | --- |
| [01 · Continuous Integration](workflows/01-ci.yml) | GCC, Clang, MSVC, macOS, Windows, Linux (incl. 32 bits e sanitizers), **FreeBSD**, headers e Sort | Push, PR, manual |
| [02 · Platform Compatibility](workflows/02-platforms.yml) | OpenBSD, NetBSD, DragonFly BSD, Fedora, Arch, Void glibc/musl, Debian representativo do Crostini e Nix toolchain | Push relevante, PR relevante, semanal, manual |
| [03 · Security Analysis](workflows/03-security.yml) | CodeQL | Push main, PR main, manual |
| [04 · Repository Maintenance](workflows/04-maintenance.yml) | Entropy e badge via PR automatizado | Push main relevante, manual |

## Como ler

- Na **01**, o job `CI / Gate` resume todos os jobs obrigatórios daquela execução. O FreeBSD é executado aqui e não se repete na matriz de compatibilidade.
- Na **02**, o job `Compatibility / Summary` aponta quais plataformas aprovaram. Cada BSD compila e **executa** portable C11, testes POSIX e a suíte Input na própria VM.
- Os erros por módulo são encontrados nas etapas/logs dos jobs; scripts sob `.github/scripts/` concentram a implementação dos testes.
- `skipped` e `cancelled` **não são** tratados como testes aprovados.
- OpenHarmony tem um verificador **compile-only** em `Tools/ohos/`, sem SDK hospedado ou certificação de hardware.
- O Debian container é uma aproximação do Crostini, não um teste real de ChromeOS. Nix toolchain não é uma VM NixOS.

## Branch protection

Configure como check obrigatório **CI / Gate** após verificar que o novo workflow rodou na branch relevante. Se houver regras antigas apontando para nomes de jobs removidos, atualize-as na configuração do repositório. Os arquivos do workflow não alteram essa configuração automaticamente.

## Duplicação e execução

Os workflows antigos `ci.yml`, `sort.yml`, `input-bsd.yml`, `portable-bsd.yml`, `linux-distros.yml`, `codeql.yml` e `entropy.yml` foram consolidados nos quatro arquivos numerados. O histórico das execuções antigas permanece no GitHub.

CI principal não cancela commits anteriores em push; pull requests cancelam apenas execuções obsoletas do mesmo fluxo. Assim ficam mais fáceis de rastrear regressões por commit.
