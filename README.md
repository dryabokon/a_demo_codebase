# Simple Experiment

A demo codebase for running experimental workflows with automated CI/CD integration.

## Overview

This project demonstrates a simple experiment framework that:
- Generates random images using OpenCV and NumPy
- Tracks experiment runs with sequential numbering
- Measures and reports execution time
- Automatically archives results
- Integrates with GitHub Actions for CI/CD

## Features

- **Automated Experiment Execution**: Runs experiments with automatic output folder management
- **Performance Tracking**: Measures and displays total execution duration
- **Results Archiving**: Automatically creates numbered run folders for each experiment
- **CI/CD Integration**: GitHub Actions workflow triggers on every push to main
- **Metadata Generation**: Saves run metadata (timestamps, commit info, run numbers)

## Project Structure

```
a_demo_codebase/
├── src/
│   ├── main.py                    # Main entry point with timing
│   ├── config.py                  # Experiment configuration
│   ├── config_base.py             # Base configuration class
│   └── utils_experiment_ops.py    # Experiment operations and utilities
├── .github/
│   └── workflows/
│       └── run-experiment.yml     # GitHub Actions workflow
├── prompts/
│   ├── jenkins-retrieve-results.md        # Jenkins API documentation
│   └── github-actions-retrieve-results.md # GitHub Actions API documentation
├── runs/                          # Experiment output folders (auto-generated)
├── pyproject.toml                 # Project dependencies
└── README.md                      # This file
```

## Requirements

- Python 3.10 - 3.13
- Dependencies:
  - `opencv-python` - Image processing
  - `numpy` - Numerical operations
  - `pandas` - Data manipulation
  - `pyyaml` - YAML configuration
  - `tabulate` - Table formatting

## Installation

### Using uv (recommended)

```bash
# Install uv package manager
pip install uv

# Sync dependencies
uv sync

# Run the experiment
uv run python src/main.py
```

### Using pip

```bash
# Install dependencies
pip install opencv-python numpy pandas pyyaml tabulate

# Run the experiment
python src/main.py
```

## Usage

### Running Locally

```bash
# Navigate to project directory
cd a_demo_codebase

# Run the experiment
python src/main.py
```

Output:
```
Check results in ./runs/001/
Total duration: 0.15 seconds
```

### What It Does

1. Creates a sequential run folder (e.g., `runs/001/`, `runs/002/`, etc.)
2. Generates a random 640x480 RGB image
3. Saves the image as `image.png` in the run folder
4. Reports execution time

### Running via GitHub Actions

The workflow automatically runs on every push to the `main` branch:

1. Sets up Python 3.13 on Windows runner
2. Installs dependencies using uv
3. Executes the experiment
4. Saves run metadata (run ID, commit hash, timestamp)
5. Uploads artifacts (output folder + metadata)

View workflow runs: https://github.com/dryabokon/a_demo_codebase/actions

## Configuration

Experiment configuration is managed in [src/config.py](src/config.py):

```python
class cnfg_experiment(config_base.cnfg_base):
    param_a = 'a'
```

Base configuration and additional parameters are defined in [src/config_base.py](src/config_base.py).

## CI/CD Integration

### GitHub Actions

- **Workflow**: [Run Experiment](.github/workflows/run-experiment.yml)
- **Trigger**: Push to main branch
- **Runner**: Windows-latest
- **Artifacts**: Experiment results and metadata (90-day retention)

### Jenkins (Local)

Jenkins instance configured at `http://127.0.0.1:8080/job/test-job/`

See [prompts/jenkins-retrieve-results.md](prompts/jenkins-retrieve-results.md) for API documentation.

## Documentation

Comprehensive guides for retrieving CI/CD results:

- **[Jenkins API Guide](prompts/jenkins-retrieve-results.md)**: Complete reference for Jenkins REST API
  - Triggering builds
  - Retrieving build status and logs
  - Downloading artifacts
  - CSRF token handling

- **[GitHub Actions API Guide](prompts/github-actions-retrieve-results.md)**: Complete reference for GitHub Actions API
  - Listing workflow runs
  - Getting job details and steps
  - Downloading artifacts and logs
  - Triggering workflows programmatically

## Output

Each experiment run creates a new folder in `./runs/` with sequential numbering:

```
runs/
├── 001/
│   └── image.png
├── 002/
│   └── image.png
└── 003/
    └── image.png
```

## Performance

Typical execution time: **~0.15 seconds** (locally)

GitHub Actions execution time: **~40 seconds** (includes setup)

## Development

### Adding New Experiments

1. Modify [src/utils_experiment_ops.py](src/utils_experiment_ops.py) `run_experiment()` method
2. Update configuration in [src/config.py](src/config.py) if needed
3. Test locally
4. Commit and push to trigger GitHub Actions

### Running Tests

```bash
# Navigate to tests directory
cd tests/hitl

# Run test cases (if configured)
python -m pytest
```

## License

This is a demo project for educational and experimental purposes.

## Author

Created by [@dryabokon](https://github.com/dryabokon)

## Links

- **Repository**: https://github.com/dryabokon/a_demo_codebase
- **GitHub Actions**: https://github.com/dryabokon/a_demo_codebase/actions
- **Issues**: https://github.com/dryabokon/a_demo_codebase/issues
